#pragma once

#include <string>
#include <map>

#include "miniaudio.h"

enum class MuteState { UNMUTED, MUSIC_MUTED, ALL_MUTED };

class SoundEngine
{
public:
	static void init();
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
	static MuteState getMuteState();

	static bool musicEnded();
	static const std::string& currentMusicName();

protected:
	static void checkError(ma_result result, const char* description);

private:
	SoundEngine();
	~SoundEngine();

	static ma_engine _engine;
	static std::map<std::string, ma_sound*> _sounds;
	static ma_sound* _musicPlaying;
	static std::string _musicPlayingName;

	static float _musicVolume;
	static float _effectVolume;

	static MuteState _muteState;
	static float _savedMusicVolume;
	static float _savedEffectVolume;
};
