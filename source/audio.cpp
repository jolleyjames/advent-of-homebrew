#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <asndlib.h>
#include <mp3player.h>
#include <ogc/lwp.h>

#include "audio.hpp"

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
    // member values initialized in header
}

advhb::Audio::~Audio()
{
    // TODO re-implement me with new class design in mind, including threads if necessary (probably necessary)
    if (MP3Player_IsPlaying())
        MP3Player_Stop();
    if (mp3Buffer != nullptr)
        delete[] mp3Buffer;
}

bool advhb::Audio::updateVolumeBgmMP3(std::uint8_t newVolume)
{
    // TODO start critical section
    // update only while fading
    bool updated = false;
    if (state == AudioState::Fading)
    {
        volume = newVolume;
        MP3Player_Volume(static_cast<u32>(volume));
        updated = true;
    }
    // TODO end critical section
    return updated;
}

bool advhb::Audio::playBgmMP3(const std::filesystem::path &path)
{
    // TODO start critical section
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
            // TODO implement looping with thread to restart song
        }
    }
    // TODO end critical section
    return stateChange;
}

bool advhb::Audio::stopBgmMP3()
{
    // TODO start critical section
    // stop only if audio is actively playing
    bool stateChange = false;
    if (state == AudioState::Playing || state == AudioState::Fading)
    {
        state = AudioState::Stopped;
        // TODO join active threads
        MP3Player_Stop();
        delete[] mp3Buffer;
        mp3Size = 0;
        volume = MIN_VOLUME;
        stateChange = true;
    }
    // TODO end critical section
    return stateChange;
}

bool advhb::Audio::fadeBgmMP3()
{
    // TODO implement me
    return false; // TODO delete me
}
