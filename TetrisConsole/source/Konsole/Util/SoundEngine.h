#pragma once

#include <string>

enum class MuteState { Unmuted, MusicMuted, AllMuted };
enum class SoundtrackMode { Cycle, Random, TrackA, TrackB, TrackC };

class SoundEngine {
public:
    static bool init();
    static void cleanup();
    static void playMusic(const std::string &name);
    static void stopMusic();
    static void playSound(const std::string &name);
    static void pauseMusic();
    static void unpauseMusic();

    static float getMusicVolume();
    static void setMusicVolume(float volume);

    static float getEffectVolume();
    static void setEffectVolume(float volume);

    static void cycleMute();
    static void unmute();
    static MuteState getMuteState();

    static float desiredMusicVolume();
    static float desiredEffectVolume();

    static SoundtrackMode getSoundtrackMode();
    static void setSoundtrackMode(SoundtrackMode mode);

    static bool musicEnded();
    static const std::string &currentMusicName();
};
