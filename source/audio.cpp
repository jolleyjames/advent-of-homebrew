#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <asndlib.h>
#include <mp3player.h>

#include "audio.hpp"
#include "concurrency.hpp"

advhb::Audio &advhb::Audio::getInstance()
{
    /*
     * Thread-safe in C++11.
     * Section 6.7 [stmt.dcl], paragraph 4:
     * "If control enters the declaration concurrently while the variable is being initialized,
     *  the concurrent execution shall wait for completion of the initialization."
     * See discussion here:
     * https://stackoverflow.com/questions/449436/singleton-instance-declared-as-static-variable-of-getinstance-method-is-it-thre/449823#449823
     */
    static Audio instance;
    return instance;
}

advhb::Audio::Audio()
{
    // Initialise the Wii audio and MP3 subsystems
    ASND_Init();
    MP3Player_Init();
    // Initialize the mutex object, allowing re-entry
    LWP_MutexInit_with_sysexit(&mutex, true);
    // member values initialized in header
}

advhb::Audio::~Audio()
{
    stopBgmMP3();
    LWP_MutexLock_with_sysexit(mutex); // start critical section
    // join potential active threads
    if (loopThread != LWP_THREAD_NULL)
    {
        LWP_MutexUnlock_with_sysexit(mutex); // end critical section
        LWP_JoinThread_with_sysexit(loopThread, nullptr);
        LWP_MutexLock_with_sysexit(mutex); // start critical section
    }
    if (fadeThread != LWP_THREAD_NULL)
    {
        LWP_MutexUnlock_with_sysexit(mutex); // end critical section
        LWP_JoinThread_with_sysexit(fadeThread, nullptr);
        LWP_MutexLock_with_sysexit(mutex); // start critical section
    }
    if (mp3Buffer != nullptr)
        delete[] mp3Buffer;
    LWP_MutexUnlock_with_sysexit(mutex); // end critical section
    // Destroy the mutex object
    LWP_MutexDestroy_with_sysexit(mutex);
}

bool advhb::Audio::updateVolumeBgmMP3(std::uint8_t newVolume)
{
    LWP_MutexLock_with_sysexit(mutex); // start critical section
    // update only while fading
    bool updated = false;
    if (state == AudioState::Fading)
    {
        volume = newVolume;
        MP3Player_Volume(static_cast<u32>(volume));
        updated = true;
    }
    LWP_MutexUnlock_with_sysexit(mutex); // end critical section
    return updated;
}

void *advhb::Audio::loopThreadAction(void *arg)
{
    Audio *audioPtr = static_cast<Audio *>(arg);
    while (true)
    {
        LWP_MutexLock_with_sysexit(audioPtr->mutex); // start critical section
        if (audioPtr->state == AudioState::Playing)
        {
            if (!MP3Player_IsPlaying())
                MP3Player_PlayBuffer(audioPtr->mp3Buffer, audioPtr->mp3Size, nullptr);
            LWP_MutexUnlock_with_sysexit(audioPtr->mutex); // end critical section
            usleep(500 * 1000);                            // recheck in a half-second
        }
        else
        {
            audioPtr->loopThread = LWP_THREAD_NULL;
            LWP_MutexUnlock_with_sysexit(audioPtr->mutex); // end critical section
            break;
        }
    }
    return nullptr;
}

bool advhb::Audio::playBgmMP3(const std::filesystem::path &path)
{
    LWP_MutexLock_with_sysexit(mutex); // start critical section
    // play only if audio is already stopped
    bool stateChange = false;
    if (state == AudioState::Stopped)
    {
        // safety checks
        if (MP3Player_IsPlaying())
            MP3Player_Stop();
        if (mp3Buffer != nullptr)
        {
            delete[] mp3Buffer;
            mp3Buffer = nullptr;
            mp3Size = 0;
        }
        // track errors when loading MP3
        bool mp3LoadError = false;
        // Load MP3 buffer and update size
        try
        {
            mp3Size = std::filesystem::file_size(path);
        }
        catch (std::filesystem::filesystem_error &e)
        {
            // nbd if we can't play the background music
            mp3LoadError = true;
        }
        if (!mp3LoadError)
        {
            try
            {
                mp3Buffer = new std::uint8_t[mp3Size];
            }
            catch (std::bad_alloc &e)
            {
                // again, nbd if we can't play the background music
                mp3Size = 0;
                mp3LoadError = true;
            }
        }
        // load the MP3
        if (!mp3LoadError)
        {
            std::fstream in(path, std::ios_base::binary | std::ios_base::in);
            if (in.is_open())
            {
                in.read(reinterpret_cast<char *>(mp3Buffer), mp3Size);
                if (in.fail())
                {
                    delete[] mp3Buffer;
                    mp3Buffer = nullptr;
                    mp3Size = 0;
                    mp3LoadError = true;
                }
                else
                {
                    in.close();
                }
            }
            else
            {
                delete[] mp3Buffer;
                mp3Buffer = nullptr;
                mp3Size = 0;
                mp3LoadError = true;
            }
        }
        // set volume, start the music, and update state
        if (!mp3LoadError)
        {
            volume = MAX_VOLUME;
            MP3Player_Volume(static_cast<u32>(volume));
            MP3Player_PlayBuffer(mp3Buffer, mp3Size, nullptr);
            state = AudioState::Playing;
            stateChange = true;
            // create a thread to restart if the end of the song has been reached
            LWP_CreateThread_with_sysexit(
                &loopThread,
                loopThreadAction,
                this,
                nullptr,
                0,
                THREAD_PRIORITY);
        }
    }
    LWP_MutexUnlock_with_sysexit(mutex); // end critical section
    return stateChange;
}

bool advhb::Audio::stopBgmMP3()
{
    LWP_MutexLock_with_sysexit(mutex); // start critical section
    // stop only if audio is actively playing
    bool stateChange = false;
    if (state == AudioState::Playing || state == AudioState::Fading)
    {
        state = AudioState::Stopped;
        // join active threads before freeing buffer
        if (loopThread != LWP_THREAD_NULL)
        {
            LWP_MutexUnlock_with_sysexit(mutex); // end critical section
            LWP_JoinThread_with_sysexit(loopThread, nullptr);
            LWP_MutexLock_with_sysexit(mutex); // start critical section
        }
        if (fadeThread != LWP_THREAD_NULL)
        {
            LWP_MutexUnlock_with_sysexit(mutex); // end critical section
            LWP_JoinThread_with_sysexit(fadeThread, nullptr);
            LWP_MutexLock_with_sysexit(mutex); // start critical section
        }
        MP3Player_Stop();
        delete[] mp3Buffer;
        mp3Size = 0;
        volume = MIN_VOLUME;
        stateChange = true;
    }
    LWP_MutexUnlock_with_sysexit(mutex); // end critical section
    return stateChange;
}

void *advhb::Audio::fadeThreadAction(void *arg)
{
    Audio *audioPtr = static_cast<Audio *>(arg);
    while (true)
    {
        LWP_MutexLock_with_sysexit(audioPtr->mutex); // start critical section
        if (audioPtr->state == AudioState::Fading && audioPtr->volume > MIN_VOLUME)
        {
            audioPtr->volume--;
            MP3Player_Volume(audioPtr->volume);
            LWP_MutexUnlock_with_sysexit(audioPtr->mutex); // end critical section
            // carry out fade effect over 1 second
            const int usecSleep = 1000 * 1000 / MAX_VOLUME;
            usleep(usecSleep);
        }
        else
        {
            audioPtr->fadeThread = LWP_THREAD_NULL;
            LWP_MutexUnlock_with_sysexit(audioPtr->mutex); // end critical section
            audioPtr->stopBgmMP3();
            break;
        }
    }
    return nullptr;
}

bool advhb::Audio::fadeBgmMP3()
{
    LWP_MutexLock_with_sysexit(mutex); // start critical section
    // fade only if audio is actively playing
    bool stateChange = false;
    if (state == AudioState::Playing)
    {
        state = AudioState::Fading;
        stateChange = true;
        // create a thread gradually reduce volume for a fade-out effect
        LWP_CreateThread_with_sysexit(
            &fadeThread,
            fadeThreadAction,
            this,
            nullptr,
            0,
            THREAD_PRIORITY);
    }
    LWP_MutexUnlock_with_sysexit(mutex); // end critical section
    return stateChange;
}
