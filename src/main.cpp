#include <raylib.h>
#include "Game.h"
#include "Input.h"
#include "Render.h"
#include "Constants.h" // 確保 Scene enum 可用
#include <ctime>
#include <iostream>
#include "FrameProfile.h"
#include <string> // For std::string

int main(int argc, char** argv)
{
    bool verbose = false;
    std::string profilePath;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--verbose") verbose = true;
        else if (arg == "--profile" && i + 1 < argc) profilePath = argv[++i];
        else {
            std::cerr << "Usage: music_game [--verbose] [--profile output.csv]\n";
            return 1;
        }
    }
    std::vector<double> frameSamples;
    if (!profilePath.empty()) frameSamples.reserve(120000);
    SetTraceLogLevel(verbose ? LOG_ALL : LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RCade");

    InitAudioDevice(); // 🔴 在所有音訊操作之前初始化音訊設備
    SetTargetFPS(60);
    srand(time(0));

    LoadGameAssets(); // 加載全局紋理等

    GameState gameState;
    InputState inputState;

    InitializeSongDatabase(gameState.songDb);
    InitializeInput(inputState); // 加載點擊音效等

    gameState.introStartTime = GetTime();
    // gameState.currentScene 默認為 Scene::Intro

    while (!WindowShouldClose())
    {
        // Time the complete gameplay frame, including EndDrawing's frame pacing.
        const double frameStart = GetTime();
        const bool sampleFrame = !profilePath.empty() &&
            gameState.currentScene == Scene::Playing &&
            !gameState.isCountdown && !gameState.gameEnding;
        // 更新邏輯
        switch (gameState.currentScene)
        {
        case Scene::Intro:
            // Intro 的邏輯主要在 DrawIntroScene 中基於時間驅動
            break;
        case Scene::MainMenu:
            // MainMenu 的邏輯主要在 DrawMainMenu 中
            break;
        case Scene::SongSelect:
            // SongSelect 的邏輯主要在 DrawSongSelect 中
            break;
        case Scene::Playing:
            UpdateGame(gameState); // UpdateGame 會處理遊戲邏輯和場景切換到 ScoreScreen

            // 🔴 添加日誌來檢查 HandleInput 的調用條件
            TraceLog(LOG_DEBUG, "MainLoop - Playing Scene: isCountdown=%s, gameEnding=%s, AudioValid=%s",
                     gameState.isCountdown ? "T":"F",
                     gameState.gameEnding ? "T":"F",
                     gameState.audio.isValid ? "T":"F");

            if (!gameState.isCountdown && !gameState.gameEnding) { // 只有在遊戲進行中才處理輸入
                HandleInput(inputState, gameState);
            }
            break;
        case Scene::ScoreScreen:
            if (IsKeyPressed(KEY_ENTER)) {
                gameState.currentScene = Scene::MainMenu;
                gameState.mainMenuFirstEnter = true; // 確保主選單動畫重置
            }
            break;
        }

        // 繪製邏輯
        BeginDrawing();
        ClearBackground(BLACK); // 可以用一個基礎背景色，各場景會覆蓋

        // TraceLog(LOG_DEBUG, "MainLoop: Current scene: %d, GameEnding: %s", (int)gameState.currentScene, gameState.gameEnding ? "T":"F");


        switch (gameState.currentScene)
        {
        case Scene::Intro:
            DrawIntroScene(gameState);
            break;
        case Scene::MainMenu:
            DrawMainMenu(gameState);
            break;
        case Scene::SongSelect:
            DrawSongSelect(gameState);
            break;
        case Scene::Playing:
            DrawGame(gameState, inputState);
            break;
        case Scene::ScoreScreen:
            DrawScoreScreen(gameState);
            break;
        }

        EndDrawing();
        if (sampleFrame) frameSamples.push_back((GetTime() - frameStart) * 1000.0);
    }

    // 清理資源
    CleanupGame(gameState);     // 清理 GameState 中的資源 (如當前歌曲的 Music stream)
    UnloadGameAssets();         // 卸載 LoadGameAssets 加載的全局資源

    // 清理 InputState 中的資源 (如音效)
    if (inputState.hitSound.frameCount > 0) {
        UnloadSound(inputState.hitSound);
    }
    
    CloseAudioDevice();       // 🔴 在最後關閉音訊設備
    CloseWindow();

    if (!profilePath.empty()) {
        std::ofstream csv(profilePath);
        if (!csv) {
            std::cerr << "Cannot write profile: " << profilePath << '\n';
            return 2;
        }
        csv << "frame,milliseconds\n" << std::fixed << std::setprecision(6);
        for (std::size_t i = 0; i < frameSamples.size(); ++i)
            csv << i << ',' << frameSamples[i] << '\n';
        csv.close();
        if (!csv) { std::cerr << "Profile write failed\n"; return 2; }
        const auto s = SummarizeFrames(frameSamples);
        std::cout << "Gameplay frames=" << frameSamples.size()
                  << " log=" << (verbose ? "verbose" : "warning")
                  << " mean_ms=" << s.mean << " p95_ms=" << s.p95
                  << " p99_ms=" << s.p99 << " max_ms=" << s.maximum
                  << " over25ms=" << s.over25 << " over50ms=" << s.over50 << '\n';
    }
    return 0;
}
