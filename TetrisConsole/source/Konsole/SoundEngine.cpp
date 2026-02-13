#define MINIAUDIO_IMPLEMENTATION
#include "SoundEngine.h"
#include "media_data.h"
#include <cstring>

ma_engine SoundEngine::_engine;
map<string, ma_sound*> SoundEngine::_sounds;
ma_sound* SoundEngine::_musicPlaying = nullptr;
string SoundEngine::_musicPlayingName;

float SoundEngine::_musicVolume = 0.1f;
float SoundEngine::_effectVolume = 0.5f;

// --- Embedded VFS for serving media from compiled-in data ---

struct EmbeddedFile {
	const unsigned char* data;
	size_t size;
	size_t cursor;
};

struct EmbeddedVFS {
	ma_vfs_callbacks cb;
};

static ma_result embeddedVFS_onOpen(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile)
{
	(void)pVFS;
	if (openMode & MA_OPEN_MODE_WRITE)
		return MA_NOT_IMPLEMENTED;

	const unsigned char* data;
	size_t size;
	if (!findEmbeddedMedia(pFilePath, &data, &size))
		return MA_DOES_NOT_EXIST;

	auto* ef = new EmbeddedFile();
	ef->data = data;
	ef->size = size;
	ef->cursor = 0;
	*pFile = static_cast<ma_vfs_file>(ef);
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onOpenW(ma_vfs* pVFS, const wchar_t* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile)
{
	(void)pVFS; (void)pFilePath; (void)openMode; (void)pFile;
	return MA_NOT_IMPLEMENTED;
}

static ma_result embeddedVFS_onClose(ma_vfs* pVFS, ma_vfs_file file)
{
	(void)pVFS;
	delete static_cast<EmbeddedFile *>(file);
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onRead(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead)
{
	(void)pVFS;
	auto* ef = static_cast<EmbeddedFile *>(file);
	const size_t remaining = ef->size - ef->cursor;
	const size_t toRead = (sizeInBytes < remaining) ? sizeInBytes : remaining;
	if (toRead == 0) {
		if (pBytesRead) *pBytesRead = 0;
		return MA_AT_END;
	}
	memcpy(pDst, ef->data + ef->cursor, toRead);
	ef->cursor += toRead;
	if (pBytesRead) *pBytesRead = toRead;
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onWrite(ma_vfs* pVFS, ma_vfs_file file, const void* pSrc, size_t sizeInBytes, size_t* pBytesWritten)
{
	(void)pVFS; (void)file; (void)pSrc; (void)sizeInBytes; (void)pBytesWritten;
	return MA_NOT_IMPLEMENTED;
}

static ma_result embeddedVFS_onSeek(ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin)
{
	(void)pVFS;
	const auto ef = static_cast<EmbeddedFile *>(file);
	ma_int64 newCursor;
	switch (origin) {
		case ma_seek_origin_start:   newCursor = offset; break;
		case ma_seek_origin_current: newCursor = static_cast<ma_int64>(ef->cursor) + offset; break;
		case ma_seek_origin_end:     newCursor = static_cast<ma_int64>(ef->size) + offset; break;
		default: return MA_INVALID_ARGS;
	}
	if (newCursor < 0 || static_cast<size_t>(newCursor) > ef->size)
		return MA_INVALID_ARGS;
	ef->cursor = static_cast<size_t>(newCursor);
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onTell(ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor)
{
	(void)pVFS;
	const auto ef = static_cast<EmbeddedFile *>(file);
	*pCursor = static_cast<ma_int64>(ef->cursor);
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onInfo(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo)
{
	(void)pVFS;
	const auto ef = static_cast<EmbeddedFile *>(file);
	pInfo->sizeInBytes = ef->size;
	return MA_SUCCESS;
}

static EmbeddedVFS g_embeddedVFS;

// --- End Embedded VFS ---

static ma_sound* createStreamSound(ma_engine* engine, const char* file, bool looping)
{
	const auto sound = new ma_sound();
	if (const ma_result result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound); result != MA_SUCCESS)
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
	const auto sound = new ma_sound();
	if (const ma_result result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound); result != MA_SUCCESS)
	{
		printf("miniaudio error loading %s (%d)\n", file, result);
		delete sound;
		return nullptr;
	}
	return sound;
}

void SoundEngine::init()
{
	g_embeddedVFS.cb.onOpen  = embeddedVFS_onOpen;
	g_embeddedVFS.cb.onOpenW = embeddedVFS_onOpenW;
	g_embeddedVFS.cb.onClose = embeddedVFS_onClose;
	g_embeddedVFS.cb.onRead  = embeddedVFS_onRead;
	g_embeddedVFS.cb.onWrite = embeddedVFS_onWrite;
	g_embeddedVFS.cb.onSeek  = embeddedVFS_onSeek;
	g_embeddedVFS.cb.onTell  = embeddedVFS_onTell;
	g_embeddedVFS.cb.onInfo  = embeddedVFS_onInfo;

	ma_engine_config config = ma_engine_config_init();
	ma_resource_manager_config rmConfig = ma_resource_manager_config_init();
	rmConfig.pVFS = static_cast<ma_vfs *>(&g_embeddedVFS);

	static ma_resource_manager resourceManager;
	ma_result result = ma_resource_manager_init(&rmConfig, &resourceManager);
	checkError(result, "resource manager init");

	config.pResourceManager = &resourceManager;
	result = ma_engine_init(&config, &_engine);
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

void SoundEngine::playMusic(const string& name)
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

void SoundEngine::playSound(const string& name)
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

void SoundEngine::setMusicVolume(const float volume)
{
	_musicVolume = volume;
	if (_musicPlaying != nullptr)
		ma_sound_set_volume(_musicPlaying, volume);
}

float SoundEngine::getEffectVolume()
{
	return _effectVolume;
}

void SoundEngine::setEffectVolume(const float volume)
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

SoundEngine::SoundEngine() = default;

SoundEngine::~SoundEngine()
{
	for (auto&[fst, snd] : _sounds)
	{
		if (snd != nullptr)
		{
			ma_sound_uninit(snd);
			delete snd;
		}
	}
	ma_engine_uninit(&_engine);
}

void SoundEngine::checkError(const ma_result result, const char* description)
{
	if (result != MA_SUCCESS)
	{
		printf("miniaudio error: %s (%d)\n", description, result);
		exit(-1);
	}
}
