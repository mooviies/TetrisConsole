#pragma once

#include <string>
#include <map>

#include "miniaudio.h"

using namespace std;

class SoundEngine
{
public:
	static void init();
	static void playMusic(string name);
	static void stopMusic();
	static void playSound(string name);
	static void pauseMusic();
	static void unpauseMusic();

	static float getMusicVolume();
	static void setMusicVolume(float volume);

	static float getEffectVolume();
	static void setEffectVolume(float volume);

	static void update();

protected:
	static void checkError(ma_result result, const char* description);

private:
	SoundEngine();
	~SoundEngine();

	static ma_engine _engine;
	static map<string, ma_sound*> _sounds;
	static ma_sound* _musicPlaying;
	static string _musicPlayingName;

	static float _musicVolume;
	static float _effectVolume;
};
