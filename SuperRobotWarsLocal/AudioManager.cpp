#include "AudioManager.h"
#include <iostream>

void AudioManager::playBGM(const std::string& path, bool loop) {
    // 仮実装: 後で SDL_mixer などに置き換え
    std::cout << "[Audio] Play BGM: " << path
        << (loop ? " (loop)" : "") << std::endl;
}

void AudioManager::stopBGM() {
    std::cout << "[Audio] Stop BGM" << std::endl;
}

void AudioManager::playSE(const std::string& path) {
    std::cout << "[Audio] Play SE: " << path << std::endl;
}