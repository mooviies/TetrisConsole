#define MINIAUDIO_IMPLEMENTATION
#include "SoundEngine.h"

ma_engine SoundEngine::_engine;
map<string, ma_sound*> SoundEngine::_sounds;
ma_sound* SoundEngine::_musicPlaying = nullptr;
string SoundEngine::_musicPlayingName = "";

float SoundEngine::_musicVolume = 0.1f;
float SoundEngine::_effectVolume = 0.5f;

static ma_sound* createStreamSound(ma_engine* engine, const char* file, bool looping)
{
	ma_sound* sound = new ma_sound();
	ma_result result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound);
	if (result != MA_SUCCESS)
	{
		printf("miniaudio error loading %s (%d)\n", file, result);
		delete sound;
		return nullptr;
	}
	if (looping)
		ma_sound_set_looping(sound, MA_TRUE);
	return sound;
}

static ma_sound* createEffectSound(ma_engine* engine, const char* file)
{
	ma_sound* sound = new ma_sound();
	ma_result result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound);
	if (result != MA_SUCCESS)
	{
		printf("miniaudio error loading %s (%d)\n", file, result);
		delete sound;
		return nullptr;
	}
	return sound;
}

void SoundEngine::init()
{
	ma_result result = ma_engine_init(nullptr, &_engine);
	checkError(result, "engine init");

	// Music streams
	_sounds["A"] = createStreamSound(&_engine, "media/A.mp3", false);
	_sounds["B"] = createStreamSound(&_engine, "media/B.mp3", false);
	_sounds["C"] = createStreamSound(&_engine, "media/C.mp3", false);
	_sounds["TITLE"] = createStreamSound(&_engine, "media/title.mp3", true);
	_sounds["SCORE"] = createStreamSound(&_engine, "media/score.mp3", true);

	// Sound effects
	_sounds["LOCK"] = createEffectSound(&_engine, "media/lock.wav");
	_sounds["HARD_DROP"] = createEffectSound(&_engine, "media/harddrop.wav");
	_sounds["CLICK"] = createEffectSound(&_engine, "media/click.wav");
	_sounds["LINE_CLEAR"] = createEffectSound(&_engine, "media/lineclear.wav");
	_sounds["TETRIS"] = createEffectSound(&_engine, "media/tetris.wav");
}

void SoundEngine::playMusic(string name)
{
	ma_sound* sound = _sounds[name];
	if (sound == nullptr)
		return;

	// Stop current music
	if (_musicPlaying != nullptr)
		ma_sound_stop(_musicPlaying);

	_musicPlayingName = name;
	_musicPlaying = sound;

	ma_sound_seek_to_pcm_frame(sound, 0);
	ma_sound_set_volume(sound, _musicVolume);
	ma_sound_start(sound);
}

void SoundEngine::playSound(string name)
{
	ma_sound* sound = _sounds[name];
	if (sound == nullptr)
		return;

	ma_sound_seek_to_pcm_frame(sound, 0);
	ma_sound_set_volume(sound, _effectVolume);
	ma_sound_start(sound);
}

void SoundEngine::stopMusic()
{
	if (_musicPlaying != nullptr)
	{
		ma_sound_stop(_musicPlaying);
		_musicPlaying = nullptr;
		_musicPlayingName = "";
	}
}

void SoundEngine::pauseMusic()
{
	if (_musicPlaying != nullptr)
		ma_sound_stop(_musicPlaying);
}

void SoundEngine::unpauseMusic()
{
	if (_musicPlaying != nullptr)
		ma_sound_start(_musicPlaying);
}

float SoundEngine::getMusicVolume()
{
	return _musicVolume;
}

void SoundEngine::setMusicVolume(float volume)
{
	_musicVolume = volume;
	if (_musicPlaying != nullptr)
		ma_sound_set_volume(_musicPlaying, volume);
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
	if (_musicPlaying != nullptr)
	{
		if (ma_sound_at_end(_musicPlaying))
		{
			if (_musicPlayingName == "A")
				playMusic("B");
			else if (_musicPlayingName == "B")
				playMusic("C");
			else if (_musicPlayingName == "C")
				playMusic("A");
		}
	}
}

SoundEngine::SoundEngine()
{
}

SoundEngine::~SoundEngine()
{
	for (auto& pair : _sounds)
	{
		if (pair.second != nullptr)
		{
			ma_sound_uninit(pair.second);
			delete pair.second;
		}
	}
	ma_engine_uninit(&_engine);
}

void SoundEngine::checkError(ma_result result, const char* description)
{
	if (result != MA_SUCCESS)
	{
		printf("miniaudio error: %s (%d)\n", description, result);
		system("pause");
		exit(-1);
	}
}
