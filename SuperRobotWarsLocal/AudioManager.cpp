#include "AudioManager.h"
#include <SDL_mixer.h>
#include <iostream>
#include <algorithm>

AudioManager::AudioManager()
    : initialized_(false)
    , currentBGMId_("")
    , bgmVolume_(64)
    , seVolume_(96)
    , masterVolume_(80)
{
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
    if (initialized_) {
        std::cout << "[AudioManager] Already initialized\n";
        return true;
    }

    int flags = MIX_INIT_OGG | MIX_INIT_MP3;
    int initted = Mix_Init(flags);

    if ((initted & flags) != flags) {
        std::cerr << "[AudioManager] Mix_Init warning: " << Mix_GetError() << "\n";
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "[AudioManager] Mix_OpenAudio failed: " << Mix_GetError() << "\n";
        return false;
    }

    Mix_AllocateChannels(16);

    initialized_ = true;
    std::cout << "[AudioManager] Initialized successfully (SDL2_mixer)\n";

    applyMasterVolume();
    return true;
}

void AudioManager::shutdown() {
    if (!initialized_) return;

    std::cout << "[AudioManager] Shutting down...\n";

    stopBGM();
    for (auto& pair : bgmMap_) {
        Mix_FreeMusic(pair.second);
    }
    bgmMap_.clear();

    stopAllSE();
    for (auto& pair : seMap_) {
        Mix_FreeChunk(pair.second);
    }
    seMap_.clear();

    Mix_CloseAudio();
    Mix_Quit();

    initialized_ = false;
}

bool AudioManager::loadBGM(const std::string& id, const std::string& filepath) {
    if (!initialized_) return false;

    if (bgmMap_.find(id) != bgmMap_.end()) {
        return true;
    }

    Mix_Music* music = Mix_LoadMUS(filepath.c_str());
    if (!music) {
        std::cerr << "[AudioManager] Failed to load BGM: " << filepath
            << " - " << Mix_GetError() << "\n";
        return false;
    }

    bgmMap_[id] = music;
    std::cout << "[AudioManager] BGM loaded: " << id << "\n";
    return true;
}

void AudioManager::playBGM(const std::string& id, bool loop, int fadeInMs) {
    if (!initialized_) return;

    auto it = bgmMap_.find(id);
    if (it == bgmMap_.end()) {
        std::cerr << "[AudioManager] BGM not found: " << id << "\n";
        return;
    }

    if (currentBGMId_ == id && Mix_PlayingMusic()) {
        return;
    }

    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }

    int loops = loop ? -1 : 0;

    if (fadeInMs > 0) {
        Mix_FadeInMusic(it->second, loops, fadeInMs);
    }
    else {
        Mix_PlayMusic(it->second, loops);
    }

    currentBGMId_ = id;
    std::cout << "[AudioManager] Playing BGM: " << id << "\n";
}

void AudioManager::stopBGM(int fadeOutMs) {
    if (!initialized_ || !Mix_PlayingMusic()) return;

    if (fadeOutMs > 0) {
        Mix_FadeOutMusic(fadeOutMs);
    }
    else {
        Mix_HaltMusic();
    }

    currentBGMId_ = "";
}

void AudioManager::pauseBGM() {
    if (!initialized_ || !Mix_PlayingMusic()) return;
    Mix_PauseMusic();
}

void AudioManager::resumeBGM() {
    if (!initialized_) return;
    Mix_ResumeMusic();
}

void AudioManager::setBGMVolume(int volume) {
    bgmVolume_ = std::clamp(volume, 0, 128);
    applyMasterVolume();
}

bool AudioManager::loadSE(const std::string& id, const std::string& filepath) {
    if (!initialized_) return false;

    if (seMap_.find(id) != seMap_.end()) {
        return true;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
        std::cerr << "[AudioManager] Failed to load SE: " << filepath
            << " - " << Mix_GetError() << "\n";
        return false;
    }

    seMap_[id] = chunk;
    std::cout << "[AudioManager] SE loaded: " << id << "\n";
    return true;
}

void AudioManager::playSE(const std::string& id, int volume) {
    if (!initialized_) return;

    auto it = seMap_.find(id);
    if (it == seMap_.end()) {
        return; // エラーメッセージを出さない（ファイルがない場合は無音）
    }

    if (volume >= 0) {
        Mix_VolumeChunk(it->second, std::clamp(volume, 0, 128));
    }
    else {
        Mix_VolumeChunk(it->second, seVolume_);
    }

    Mix_PlayChannel(-1, it->second, 0);
}

void AudioManager::stopAllSE() {
    if (!initialized_) return;
    Mix_HaltChannel(-1);
}

void AudioManager::setSEVolume(int volume) {
    seVolume_ = std::clamp(volume, 0, 128);
    applyMasterVolume();
}

void AudioManager::setMasterVolume(int volume) {
    masterVolume_ = std::clamp(volume, 0, 100);
    applyMasterVolume();
}

void AudioManager::applyMasterVolume() {
    if (!initialized_) return;

    float masterRatio = masterVolume_ / 100.0f;
    int actualBGMVolume = static_cast<int>(bgmVolume_ * masterRatio);
    Mix_VolumeMusic(actualBGMVolume);

    int actualSEVolume = static_cast<int>(seVolume_ * masterRatio);
    for (auto& pair : seMap_) {
        Mix_VolumeChunk(pair.second, actualSEVolume);
    }
}

void AudioManager::preloadCommonSE() {
    std::cout << "[AudioManager] Preloading common SE...\n";

    loadSE("cursor", "assets/audio/se/cursor.wav");
    loadSE("decide", "assets/audio/se/decide.wav");
    loadSE("cancel", "assets/audio/se/cancel.wav");
    loadSE("hit", "assets/audio/se/hit.wav");
    loadSE("miss", "assets/audio/se/miss.wav");
    loadSE("critical", "assets/audio/se/critical.wav");
    loadSE("explosion", "assets/audio/se/explosion.wav");
    loadSE("beam", "assets/audio/se/beam.wav");

    std::cout << "[AudioManager] Preload complete\n";
}