#pragma once

#include <string>
#include <unordered_map>

// 前方宣言
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;
struct Mix_Chunk;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    void shutdown();

    // BGM
    bool loadBGM(const std::string& id, const std::string& filepath);
    void playBGM(const std::string& id, bool loop = true, int fadeInMs = 0);
    void stopBGM(int fadeOutMs = 0);
    void pauseBGM();
    void resumeBGM();
    void setBGMVolume(int volume);

    // SE
    bool loadSE(const std::string& id, const std::string& filepath);
    void playSE(const std::string& id, int volume = -1);
    void stopAllSE();
    void setSEVolume(int volume);

    // マスター音量
    void setMasterVolume(int volume);

    // ユーティリティ
    bool isBGMPlaying() const;
    std::string getCurrentBGM() const { return currentBGMId_; }
    void preloadCommonSE();

private:
    bool initialized_;
    std::unordered_map<std::string, Mix_Music*> bgmMap_;
    std::string currentBGMId_;
    int bgmVolume_;
    std::unordered_map<std::string, Mix_Chunk*> seMap_;
    int seVolume_;
    int masterVolume_;

    void applyMasterVolume();
};