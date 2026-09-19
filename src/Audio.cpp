#include "Audio.h"
#include <raylib.h>

void InitializeAudio(GameAudio& audio, const std::string& musicPath) {
    audio.isValid = false; // 先假設無效

    // 確保音訊設備已初始化 (通常在 main 中做一次即可，但這裡作為保險)
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_INFO, "Audio device not ready in InitializeAudio. Initializing...");
        InitAudioDevice();
    }

    TraceLog(LOG_INFO, "Attempting to initialize audio with music path: %s", musicPath.c_str());

    if (!musicPath.empty() && FileExists(musicPath.c_str())) {
        TraceLog(LOG_INFO, "Loading music stream from: %s", musicPath.c_str());
        audio.bgm = LoadMusicStream(musicPath.c_str());

        if (audio.bgm.ctxData != NULL) { // 檢查音樂流是否有效加載 (ctxData 非空表示加載可能成功)
            audio.bgm.looping = false;   // 🔴🔴 關鍵：明確設置音樂不循環
            audio.isValid = true;
            // PlayMusicStream(audio.bgm); // 不在這裡播放，由 Game.cpp 控制播放時機
            TraceLog(LOG_INFO, "Audio initialized successfully. isValid = true, looping = false. Music Length: %.2f seconds.", GetMusicTimeLength(audio.bgm));
        } else {
            TraceLog(LOG_WARNING, "Failed to load music stream (ctxData is NULL) for: %s", musicPath.c_str());
        }
    } else {
        if (musicPath.empty()) {
            TraceLog(LOG_WARNING, "Music path is empty.");
        } else {
            TraceLog(LOG_WARNING, "Music file not found: %s", musicPath.c_str());
        }
    }
}

void UpdateAudio(GameAudio& audio) {
    if (audio.isValid) {
        //TraceLog(LOG_DEBUG, "Updating music stream...");
        UpdateMusicStream(audio.bgm);
    } else {
        //TraceLog(LOG_WARNING, "Audio update skipped: audio is not valid");
    }
}

void CleanupAudio(GameAudio& audio) {
    if (audio.isValid) {
        TraceLog(LOG_INFO, "Unloading music stream...");
        UnloadMusicStream(audio.bgm);
    } else {
        TraceLog(LOG_WARNING, "No music stream to unload: audio is not valid");
    }
    TraceLog(LOG_INFO, "Closing audio device...");
    //CloseAudioDevice();
}