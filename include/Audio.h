#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>
#include <string>

struct GameAudio {
    Music bgm = {0};    // 聚合初始化，將 Music 結構體清零 (所有成員設為0或NULL)
    bool isValid = false; // 明確的初始值
};

void InitializeAudio(GameAudio& audio, const std::string& musicPath);
void UpdateAudio(GameAudio& audio);
void CleanupAudio(GameAudio& audio); // 通常由 GameState 的 CleanupGame 或 main 調用

#endif