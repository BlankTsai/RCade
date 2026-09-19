#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <vector>
#include <string> // For std::string
#include "Block.h"
#include "Audio.h"       // Audio.h 應包含 GameAudio 結構的定義
#include "SongDatabase.h"
#include "Constants.h"

// GameAudio 結構體的定義應該在 Audio.h 中，這裡只是為了 GameState 引用
// 如果 GameAudio 已經在 Audio.h 中定義並正確初始化，則 GameState 中的 audio 成員會使用它。
// 假設 Audio.h 中的 GameAudio 是這樣定義的 (如果不是，請修改 Audio.h):
/*
// In Audio.h
struct GameAudio {
    Music bgm = {0};    // 聚合初始化，將 Music 結構體清零
    bool isValid = false; // 明確的初始值
};
*/


struct GameState
{
    // Gameplay state
    std::vector<Block> blocks;
    int score;
    double nextSpawnTime;
    float spawnInterval;
    std::string currentSong;
    double musicStartTime;
    float currentBpm;

    // Audio related
    GameAudio audio; // 🔴 GameAudio 結構體本身應在 Audio.h 中確保其成員有默認值

    // Song database
    SongDatabase songDb;

    // Countdown state
    bool isCountdown = true;
    double countdownStartTime = 0;
    float countdownDuration = 4.0f;

    // Game ending state
    bool musicFinished = false;
    bool allBlocksCleared = false;
    bool gameEnding = false;
    double gameEndTime = 0.0;

    // Scene management
    Scene currentScene = Scene::Intro;
    double introStartTime = 0.0;

    bool mainMenuFirstEnter = true;
    bool songSelectFirstEnter = true;
    bool transitioning = false;

    // 用於顯示判定結果的成員
    HitRating lastHitRating = HitRating::NONE;
    float hitRatingDisplayTime = 0.0f;      // 判定結果在屏幕上顯示的剩餘時間 (秒)
    static constexpr float HIT_RATING_DURATION = 0.5f; // 判定結果顯示持續時間 (秒)
};

// Function Declarations
void InitializeGame(GameState &state, const std::string &songName = "LoveTheWorld");
void UpdateGame(GameState &state);
void CleanupGame(GameState &state);

#endif // GAME_H