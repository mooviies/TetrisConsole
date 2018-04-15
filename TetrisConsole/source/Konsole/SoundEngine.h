#pragma once

#include <string>
#include <map>

#include "fmod\fmod.hpp"

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
	static void checkFMODError(FMOD_RESULT result);

private:
	SoundEngine();
	~SoundEngine();

	static FMOD::System *_system;
	static map<string, FMOD::Sound*> _sounds;
	static map <string, FMOD::Channel*> _channels;
	static FMOD::Channel* _channelPlaying;
	static string _musicPlaying;

	static float _musicVolume;
	static float _effectVolume;
};

