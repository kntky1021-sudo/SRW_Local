#pragma once

#include <string>

/**
 * AudioManager
 * ・BGM / SE の再生管理
 * ・将来的に SDL_mixer などを統合
 */
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    // BGM のロードと再生
    void playBGM(const std::string& path, bool loop = true);

    // BGM の停止
    void stopBGM();

    // 効果音の再生
    void playSE(const std::string& path);
};