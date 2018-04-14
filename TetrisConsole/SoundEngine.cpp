#include "stdafx.h"
#include "SoundEngine.h"

#include "fmod\fmod.hpp"
#include "fmod\fmod_errors.h"

FMOD::System *SoundEngine::_system = NULL;
map<string, FMOD::Sound*> SoundEngine::_sounds;
map <string, FMOD::Channel*> SoundEngine::_channels;
FMOD::Channel* SoundEngine::_channelPlaying = NULL;
string SoundEngine::_musicPlaying = "";

float SoundEngine::_musicVolume = 0.1;
float SoundEngine::_effectVolume = 0.5;

void SoundEngine::init()
{
	FMOD_RESULT result;
	_system = NULL;

	result = FMOD::System_Create(&_system);
	checkFMODError(result);

	result = _system->init(512, FMOD_INIT_NORMAL, 0);
	checkFMODError(result);

	_sounds["A"] = NULL;
	_sounds["B"] = NULL;
	_sounds["C"] = NULL;
	_sounds["TITLE"] = NULL;
	_sounds["SCORE"] = NULL;

	_sounds["LOCK"] = NULL;
	_sounds["HARD_DROP"] = NULL;
	_sounds["CLICK"] = NULL;
	_sounds["LINE_CLEAR"] = NULL;
	_sounds["TETRIS"] = NULL;

	result = _system->createStream("media/A.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &_sounds["A"]);
	checkFMODError(result);

	result = _system->createStream("media/B.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &_sounds["B"]);
	checkFMODError(result);

	result = _system->createStream("media/C.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &_sounds["C"]);
	checkFMODError(result);

	result = _system->createStream("media/title.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &_sounds["TITLE"]);
	checkFMODError(result);

	result = _system->createStream("media/score.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &_sounds["SCORE"]);
	checkFMODError(result);

	result = _system->createSound("media/lock.wav", FMOD_DEFAULT, 0, &_sounds["LOCK"]);
	checkFMODError(result);

	result = _system->createSound("media/harddrop.wav", FMOD_DEFAULT, 0, &_sounds["HARD_DROP"]);
	checkFMODError(result);

	result = _system->createSound("media/click.wav", FMOD_DEFAULT, 0, &_sounds["CLICK"]);
	checkFMODError(result);

	result = _system->createSound("media/lineclear.wav", FMOD_DEFAULT, 0, &_sounds["LINE_CLEAR"]);
	checkFMODError(result);

	result = _system->createSound("media/tetris.wav", FMOD_DEFAULT, 0, &_sounds["TETRIS"]);
	checkFMODError(result);
}

void SoundEngine::playMusic(string name)
{
	if (_channels[name] == NULL)
	{
		_channels[name] = NULL;
		FMOD_RESULT result = _system->playSound(_sounds[name], 0, true, &_channels[name]);
		checkFMODError(result);
	}

	if (_channelPlaying != NULL)
	{
		_channelPlaying->setPaused(true);
		_channelPlaying->setMute(true);
		_channels[_musicPlaying] = NULL;
	}
	
	_musicPlaying = name;
	_channelPlaying = _channels[name];
	_channelPlaying->setMute(false);
	_channelPlaying->setVolume(_musicVolume);
	_channelPlaying->setPaused(false);
}

void SoundEngine::playSound(string name)
{
	FMOD::Channel* channel = NULL;
	FMOD_RESULT result = _system->playSound(_sounds[name], 0, false, &channel);
	channel->setVolume(_effectVolume);
	checkFMODError(result);
}

void SoundEngine::stopMusic()
{
	if (_channelPlaying != NULL)
	{
		_channelPlaying->setPaused(true);
		_channelPlaying = NULL;
		_channels[_musicPlaying] = NULL;
	}
}

void SoundEngine::pauseMusic()
{
	if (_channelPlaying != NULL)
		_channelPlaying->setPaused(true);
}

void SoundEngine::unpauseMusic()
{
	if (_channelPlaying != NULL)
		_channelPlaying->setPaused(false);
}


float SoundEngine::getMusicVolume()
{
	return _musicVolume;
}

void SoundEngine::setMusicVolume(float volume)
{
	if (_channelPlaying != NULL)
	{
		_musicVolume = volume;
		_channelPlaying->setVolume(volume);
	}
}

float SoundEngine::getEffectVolume()
{
	return _effectVolume;
}

void SoundEngine::setEffectVolume(float volume)
{
	_effectVolume = volume;
}

void SoundEngine::update()
{
	_system->update();
	if (_channelPlaying != NULL)
	{
		bool isPlaying;
		_channelPlaying->isPlaying(&isPlaying);
		if (!isPlaying)
		{
			if (_musicPlaying == "A")
			{
				playMusic("B");
			}
			else if (_musicPlaying == "B")
			{
				playMusic("C");
			}
			else if (_musicPlaying == "C")
			{
				playMusic("A");
			}
		}
	}
}

SoundEngine::SoundEngine()
{
}


SoundEngine::~SoundEngine()
{
	_sounds["A"]->release();
	_sounds["B"]->release();
	_sounds["C"]->release();
	_sounds["TITLE"]->release();
	_sounds["SCORE"]->release();
	_sounds["LOCK"]->release();
	_system->release();
}

void SoundEngine::checkFMODError(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		system("pause");
		exit(-1);
	}
}
