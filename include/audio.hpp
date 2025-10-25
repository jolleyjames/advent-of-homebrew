#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <cstdint>
#include <filesystem>
#include <asndlib.h>

namespace advhb {

    enum class AudioState { Stopped, Playing, Fading };

    class Audio {
    private:
        // singleton pattern
        Audio();
        Audio(const Audio&) = delete;
        Audio(Audio&&) = delete;
        Audio& operator=(const Audio&) = delete;
        Audio& operator=(Audio&&) = delete;
        ~Audio();
        // Wii MP3 subsystem volume ranges from 0 to 255, the full range of unsigned 8-bit integers.
        std::uint8_t volume = MIN_VOLUME;
        AudioState state = AudioState::Stopped;
        std::uint8_t* mp3Buffer = nullptr;
        std::uintmax_t mp3Size = 0;
        // Restrict access to set bgm volume. Used internally for fade-out effect.
        bool updateVolumeBgmMP3(std::uint8_t newVolume);
        
        //TODO track active threads

    public:
        static Audio& getInstance();
        bool playBgmMP3(const std::filesystem::path& path);
        bool stopBgmMP3();
        bool fadeBgmMP3();
    };

}

#endif /*AUDIO_HPP*/
