#pragma once

#include <string>
#include <map>

#include "fmod\fmod.hpp"

namespace konsole
{
	class SoundEngine
	{
	public:
		static void init();
		static void playMusic(std::string name);
		static void stopMusic();
		static void playSound(std::string name);
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
		static std::map<std::string, FMOD::Sound*> _sounds;
		static std::map <std::string, FMOD::Channel*> _channels;
		static FMOD::Channel* _channelPlaying;
		static std::string _musicPlaying;

		static float _musicVolume;
		static float _effectVolume;
	};
}