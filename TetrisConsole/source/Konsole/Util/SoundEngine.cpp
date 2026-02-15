#define MINIAUDIO_IMPLEMENTATION
#include "SoundEngine.h"
#include "miniaudio.h"
#include "media_data.h"
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace std;

// --- RAII wrapper for ma_sound pointers ---

struct MaSoundDeleter {
	void operator()(ma_sound* s) const {
		if (s) {
			ma_sound_uninit(s);
			delete s;
		}
	}
};
using MaSoundPtr = unique_ptr<ma_sound, MaSoundDeleter>;

// --- File-scope state (replaces former private static members) ---

static ma_engine s_engine;
static map<string, MaSoundPtr> s_sounds;
static ma_sound* s_musicPlaying = nullptr;  // non-owning; points into s_sounds
static string s_musicPlayingName;

static float s_musicVolume = 0.1f;
static float s_effectVolume = 0.5f;

static auto s_muteState = MuteState::UNMUTED;
static float s_savedMusicVolume = 0.1f;
static float s_savedEffectVolume = 0.5f;

// --- Embedded VFS for serving media from compiled-in data ---

struct EmbeddedFile {
	const unsigned char* data;
	size_t size;
	size_t cursor;
};

struct EmbeddedVFS {
	ma_vfs_callbacks cb;
};

static ma_result embeddedVFS_onOpen(ma_vfs* pVFS, const char* pFilePath, const ma_uint32 openMode, ma_vfs_file* pFile)
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

static ma_result embeddedVFS_onOpenW(ma_vfs* pVFS, const wchar_t* pFilePath, const ma_uint32 openMode, ma_vfs_file* pFile)
{
	(void)pVFS; (void)pFilePath; (void)openMode; (void)pFile;
	return MA_NOT_IMPLEMENTED;
}

static ma_result embeddedVFS_onClose(ma_vfs* pVFS, const ma_vfs_file file)
{
	(void)pVFS;
	delete static_cast<EmbeddedFile *>(file);
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onRead(ma_vfs* pVFS, const ma_vfs_file file, void* pDst, const size_t sizeInBytes, size_t* pBytesRead)
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

static ma_result embeddedVFS_onWrite(ma_vfs* pVFS, const ma_vfs_file file, const void* pSrc, const size_t sizeInBytes, size_t* pBytesWritten)
{
	(void)pVFS; (void)file; (void)pSrc; (void)sizeInBytes; (void)pBytesWritten;
	return MA_NOT_IMPLEMENTED;
}

static ma_result embeddedVFS_onSeek(ma_vfs* pVFS, const ma_vfs_file file, const ma_int64 offset, const ma_seek_origin origin)
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

static ma_result embeddedVFS_onTell(ma_vfs* pVFS, const ma_vfs_file file, ma_int64* pCursor)
{
	(void)pVFS;
	const auto ef = static_cast<EmbeddedFile *>(file);
	*pCursor = static_cast<ma_int64>(ef->cursor);
	return MA_SUCCESS;
}

static ma_result embeddedVFS_onInfo(ma_vfs* pVFS, const ma_vfs_file file, ma_file_info* pInfo)
{
	(void)pVFS;
	const auto ef = static_cast<EmbeddedFile *>(file);
	pInfo->sizeInBytes = ef->size;
	return MA_SUCCESS;
}

static EmbeddedVFS g_embeddedVFS;
static ma_resource_manager g_resourceManager;

// --- End Embedded VFS ---

static MaSoundPtr createStreamSound(ma_engine* engine, const char* file, const bool looping)
{
	auto* raw = new ma_sound();
	if (const ma_result result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_STREAM, nullptr, nullptr, raw); result != MA_SUCCESS)
	{
		cerr << "miniaudio error loading " << file << " (" << result << ")" << endl;
		delete raw;
		return nullptr;
	}
	if (looping)
		ma_sound_set_looping(raw, MA_TRUE);
	return MaSoundPtr(raw);
}

static MaSoundPtr createEffectSound(ma_engine* engine, const char* file)
{
	auto* raw = new ma_sound();
	if (const ma_result result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_DECODE, nullptr, nullptr, raw); result != MA_SUCCESS)
	{
		cerr << "miniaudio error loading " << file << " (" << result << ")" << endl;
		delete raw;
		return nullptr;
	}
	return MaSoundPtr(raw);
}

bool SoundEngine::init()
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

	ma_result result = ma_resource_manager_init(&rmConfig, &g_resourceManager);
	if (result != MA_SUCCESS) {
		cerr << "miniaudio error: resource manager init (" << result << ")" << endl;
		return false;
	}

	config.pResourceManager = &g_resourceManager;
	result = ma_engine_init(&config, &s_engine);
	if (result != MA_SUCCESS) {
		cerr << "miniaudio error: engine init (" << result << ")" << endl;
		ma_resource_manager_uninit(&g_resourceManager);
		return false;
	}

	// Music streams
	s_sounds["A"] = createStreamSound(&s_engine, "media/A.mp3", false);
	s_sounds["B"] = createStreamSound(&s_engine, "media/B.mp3", false);
	s_sounds["C"] = createStreamSound(&s_engine, "media/C.mp3", false);
	s_sounds["TITLE"] = createStreamSound(&s_engine, "media/title.mp3", true);
	s_sounds["SCORE"] = createStreamSound(&s_engine, "media/score.mp3", true);

	// Sound effects
	s_sounds["LOCK"] = createEffectSound(&s_engine, "media/lock.wav");
	s_sounds["HARD_DROP"] = createEffectSound(&s_engine, "media/harddrop.wav");
	s_sounds["CLICK"] = createEffectSound(&s_engine, "media/click.wav");
	s_sounds["LINE_CLEAR"] = createEffectSound(&s_engine, "media/lineclear.wav");
	s_sounds["TETRIS"] = createEffectSound(&s_engine, "media/tetris.wav");

	return true;
}

void SoundEngine::cleanup()
{
	s_musicPlaying = nullptr;
	s_sounds.clear();
	ma_engine_uninit(&s_engine);
	ma_resource_manager_uninit(&g_resourceManager);
}

void SoundEngine::playMusic(const string& name)
{
	auto it = s_sounds.find(name);
	if (it == s_sounds.end() || it->second == nullptr)
		return;
	ma_sound* sound = it->second.get();

	// Stop current music
	if (s_musicPlaying != nullptr)
		ma_sound_stop(s_musicPlaying);

	s_musicPlayingName = name;
	s_musicPlaying = sound;

	ma_sound_seek_to_pcm_frame(sound, 0);
	ma_sound_set_volume(sound, s_musicVolume);
	ma_sound_start(sound);
}

void SoundEngine::playSound(const string& name)
{
	if (s_muteState == MuteState::ALL_MUTED)
		return;

	auto it = s_sounds.find(name);
	if (it == s_sounds.end() || it->second == nullptr)
		return;
	ma_sound* sound = it->second.get();

	ma_sound_seek_to_pcm_frame(sound, 0);
	ma_sound_set_volume(sound, s_effectVolume);
	ma_sound_start(sound);
}

void SoundEngine::stopMusic()
{
	if (s_musicPlaying != nullptr)
	{
		ma_sound_stop(s_musicPlaying);
		s_musicPlaying = nullptr;
		s_musicPlayingName = "";
	}
}

void SoundEngine::pauseMusic()
{
	if (s_musicPlaying != nullptr)
		ma_sound_stop(s_musicPlaying);
}

void SoundEngine::unpauseMusic()
{
	if (s_musicPlaying != nullptr)
		ma_sound_start(s_musicPlaying);
}

float SoundEngine::getMusicVolume()
{
	return s_musicVolume;
}

void SoundEngine::setMusicVolume(const float volume)
{
	s_musicVolume = volume;
	if (s_musicPlaying != nullptr)
		ma_sound_set_volume(s_musicPlaying, volume);
}

float SoundEngine::getEffectVolume()
{
	return s_effectVolume;
}

void SoundEngine::setEffectVolume(const float volume)
{
	s_effectVolume = volume;
}

bool SoundEngine::musicEnded()
{
	return s_musicPlaying != nullptr && ma_sound_at_end(s_musicPlaying);
}

const string& SoundEngine::currentMusicName()
{
	return s_musicPlayingName;
}

void SoundEngine::cycleMute()
{
	switch (s_muteState) {
		case MuteState::UNMUTED:
			s_savedMusicVolume = s_musicVolume;
			setMusicVolume(0.0f);
			s_muteState = MuteState::MUSIC_MUTED;
			break;
		case MuteState::MUSIC_MUTED:
			s_savedEffectVolume = s_effectVolume;
			s_effectVolume = 0.0f;
			s_muteState = MuteState::ALL_MUTED;
			break;
		case MuteState::ALL_MUTED:
			setMusicVolume(s_savedMusicVolume);
			s_effectVolume = s_savedEffectVolume;
			s_muteState = MuteState::UNMUTED;
			break;
	}
}

MuteState SoundEngine::getMuteState()
{
	return s_muteState;
}
