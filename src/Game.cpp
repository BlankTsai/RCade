#include "Game.h"
#include "Block.h"
#include <ctime>
#include <raylib.h>

void InitializeGame(GameState &state, const std::string &songName)
{
    TraceLog(LOG_INFO, "InitializeGame: Called with songName: '%s'", songName.c_str());
    state.score = 0;
    float travelTime = JUDGE_LINE_Y / BLOCK_SPEED;
    state.nextSpawnTime = travelTime;
    state.blocks.clear();
    state.currentSong = songName;

    state.isCountdown = true;
    state.countdownStartTime = GetTime();

    state.musicFinished = false;
    state.allBlocksCleared = false;
    state.gameEnding = false;
    state.gameEndTime = 0.0;

    TraceLog(LOG_INFO, "InitializeGame: Checking previous audio state. isValid: %s, bgm.ctxData: %p",
             state.audio.isValid ? "true" : "false", state.audio.bgm.ctxData);

    // 在加載新音樂前，先卸載舊的音樂流 (如果存在且有效)
    if (state.audio.isValid && state.audio.bgm.ctxData != NULL)
    { // 🔴 增加 ctxData 檢查
        TraceLog(LOG_INFO, "InitializeGame: Previous audio is valid and ctxData is not NULL. Attempting to stop and unload.");

        TraceLog(LOG_INFO, "InitializeGame: Calling StopMusicStream...");
        if (IsMusicStreamPlaying(state.audio.bgm))
        { // 只在播放時停止
            StopMusicStream(state.audio.bgm);
            TraceLog(LOG_INFO, "InitializeGame: StopMusicStream called and finished.");
        }
        else
        {
            TraceLog(LOG_INFO, "InitializeGame: Previous music stream was not playing, skipping StopMusicStream.");
        }

        TraceLog(LOG_INFO, "InitializeGame: Calling UnloadMusicStream...");
        UnloadMusicStream(state.audio.bgm); // UnloadMusicStream 應該能處理未播放的流
        TraceLog(LOG_INFO, "InitializeGame: UnloadMusicStream finished.");

        state.audio.isValid = false; // 標記為無效
        state.audio.bgm = {0};       // 🔴 清零 bgm 結構體，確保它是乾淨的
    }
    else
    {
        TraceLog(LOG_INFO, "InitializeGame: No valid previous audio to stop/unload, or ctxData was NULL.");
        // 確保即使 isValid 為 false，bgm 也處於已知狀態
        if (!state.audio.isValid)
        {
            state.audio.bgm = {0}; // 如果 isValid 為 false，也清零 bgm
        }
    }

    const Song *song = GetSongByName(state.songDb, songName);
    if (song)
    {
        TraceLog(LOG_INFO, "InitializeGame: Song found: %s, BPM: %.2f, FilePath: %s, Offset: %.2f",
                 song->name.c_str(), song->bpm, song->filePath.c_str(), song->offset);
        state.spawnInterval = GetSpawnIntervalFromBPM(song->bpm);
        state.currentBpm = song->bpm;
        InitializeAudio(state.audio, song->filePath); // InitializeAudio 內部會設置 looping = false
    }
    else
    {
        TraceLog(LOG_ERROR, "InitializeGame: Song '%s' NOT FOUND in database! Using defaults.", songName.c_str());
        state.spawnInterval = 1.0f;
        state.currentBpm = 120.0f;        // 賦予明確的預設值
        InitializeAudio(state.audio, ""); // 會將 audio.isValid 設為 false
    }
    TraceLog(LOG_INFO, "InitializeGame: Finished for song: '%s'. Countdown started. Audio isValid: %s, bgm.ctxData: %p",
             songName.c_str(), state.audio.isValid ? "true" : "false", state.audio.bgm.ctxData);
}

// In Game.cpp
void UpdateGame(GameState &state)
{
    // 在函數最開頭添加這個日誌 (用於高頻監控)
    //TraceLog(LOG_ERROR, "UpdateGame: FRAME TICK. MusicTime: %.2f / %.2f, IsPlaying: %s, musicFinished: %s, Scene: %d",
            //(state.audio.isValid && state.audio.bgm.ctxData != NULL) ? GetMusicTimePlayed(state.audio.bgm) : -1.0f,
             //(state.audio.isValid && state.audio.bgm.ctxData != NULL) ? GetMusicTimeLength(state.audio.bgm) : -1.0f,
             //(state.audio.isValid && state.audio.bgm.ctxData != NULL && IsMusicStreamPlaying(state.audio.bgm)) ? "T" : "F",
             //state.musicFinished ? "T" : "F",
             //(int)state.currentScene);

    // 倒數邏輯
    if (state.isCountdown)
    {
        double elapsedCountdown = GetTime() - state.countdownStartTime;
        if (elapsedCountdown >= state.countdownDuration)
        {
            state.isCountdown = false;
            state.musicStartTime = GetTime();
            if (state.audio.isValid && state.audio.bgm.ctxData != NULL)
            {
                PlayMusicStream(state.audio.bgm);
                SetMusicVolume(state.audio.bgm, 1.0f);
                TraceLog(LOG_INFO, "UpdateGame: Music stream started. MusicStartTime: %.3f. Looping: %s. Length: %.2f",
                         state.musicStartTime, (state.audio.bgm.looping ? "true" : "false"), GetMusicTimeLength(state.audio.bgm));
            }
            else
            {
                TraceLog(LOG_WARNING, "UpdateGame: Countdown finished, but audio is not valid. Music will not play.");
            }
        }
        else
        {
            return; // 倒數期間暫停遊戲邏輯
        }
    }

    // 如果遊戲正在結束 (漸暗等)
    if (state.gameEnding)
    {
        double timeSinceEnd = GetTime() - state.gameEndTime;
        float fadeDuration = 2.0f;
        if (timeSinceEnd > fadeDuration)
        {
            state.currentScene = Scene::ScoreScreen;
            TraceLog(LOG_INFO, "UpdateGame: Fade out complete. Switching to ScoreScreen.");
        }
        return; // 在漸暗過程中，不執行後續的遊戲更新
    }

    // 更新音訊流 (必須在檢查音樂狀態前調用)
    if (state.audio.isValid && state.audio.bgm.ctxData != NULL)
    {
        UpdateAudio(state.audio); // 內部調用 UpdateMusicStream
    }
    else if (!state.musicFinished && !state.isCountdown) // 如果音訊無效但遊戲已開始
    {
        if (!state.audio.isValid || state.audio.bgm.ctxData == NULL)
        {
            TraceLog(LOG_WARNING, "UpdateGame: Audio is not valid (or ctxData NULL) during gameplay. Forcing musicFinished = true.");
            state.musicFinished = true; // 強制音樂結束，以便遊戲可以繼續到結束流程
        }
    }

    // 檢查音樂是否播放完畢
    // 🔴🔴🔴 使用新的音樂結束檢測邏輯 🔴🔴🔴
    if (state.audio.isValid && state.audio.bgm.ctxData != NULL && !state.musicFinished) {
        float playedTime = GetMusicTimePlayed(state.audio.bgm);
        float totalTime = GetMusicTimeLength(state.audio.bgm);
        bool isCurrentlyPlaying = IsMusicStreamPlaying(state.audio.bgm);

        // 高頻日誌，觀察這些值的變化 (調試完成後可註釋掉或改為 LOG_DEBUG)
        // TraceLog(LOG_DEBUG, "Music Tick: Played=%.3f, Total=%.3f, IsPlaying=%s, musicFinished=%s",
        //          playedTime, totalTime, isCurrentlyPlaying ? "T":"F", state.musicFinished ? "T":"F");

        if (!state.musicFinished) { // 只有在尚未標記完成時才檢查
            // 情況 1: 播放時間非常接近或超過總長度。
            // 這一幀 IsPlaying 可能仍然是 true。
            if (totalTime > 0 && playedTime >= totalTime - 0.05f) { // 0.05s 容差
                state.musicFinished = true;
                TraceLog(LOG_INFO, "UpdateGame: Music marked FINISHED [Case 1: Played time near/at end]. Played: %.3f, Total: %.3f, IsPlaying: %s",
                         playedTime, totalTime, isCurrentlyPlaying ? "T" : "F");
            }
            // 情況 2: Raylib 報告音樂已停止播放，並且是非循環模式。
            // 這種情況下，playedTime 可能已經被重置為 0 (根據你的日誌)。
            else if (!isCurrentlyPlaying && !state.audio.bgm.looping && totalTime > 0) {
                // 為了處理 playedTime 已經重置為0的情況，我們主要依賴 isCurrentlyPlaying 變為 false。
                state.musicFinished = true;
                TraceLog(LOG_INFO, "UpdateGame: Music marked FINISHED [Case 2: Stream stopped, not looping]. Played: %.3f, Total: %.3f",
                         playedTime, totalTime); // playedTime 此時可能是 0.00
            }
        }
    }
    // 🔴🔴🔴 音樂結束檢測邏輯結束 🔴🔴🔴


    // -------------------- 方塊生成邏輯 --------------------
    // TraceLog(LOG_DEBUG, "UpdateGame - SpawnBlock Check: musicFinished=%s, gameEnding=%s",
    //          state.musicFinished ? "TRUE" : "FALSE",
    //          state.gameEnding ? "TRUE" : "FALSE");

    const Song *songData = GetSongByName(state.songDb, state.currentSong);
    float offset = 0.0f;
    float currentSpawnInterval = state.spawnInterval;

    if (songData)
    {
        offset = songData->offset;
    }
    else
    {
        TraceLog(LOG_WARNING, "UpdateGame: Song data not found for '%s' when checking for SpawnBlock. Using default offset/interval.", state.currentSong.c_str());
    }

    if (!state.musicFinished && !state.gameEnding)
    {
        // TraceLog(LOG_INFO, "UpdateGame: Condition MET for SpawnBlock. Spawning block..."); // 已根據你的要求註釋掉
        SpawnBlock(state.blocks, state.nextSpawnTime, currentSpawnInterval, offset, state.musicStartTime);
    }
    else
    {
        // 這個日誌仍然有用，用於確認方塊生成是否按預期停止
        TraceLog(LOG_INFO, "UpdateGame: Condition NOT MET for SpawnBlock. (MusicFinished: %s, GameEnding: %s)",
                 state.musicFinished ? "TRUE" : "FALSE",
                 state.gameEnding ? "TRUE" : "FALSE");
    }
    // ----------------------------------------------------

    UpdateBlocks(state.blocks, state, state.musicStartTime);

    // 檢查是否所有方塊都已清除
    if (state.musicFinished && !state.allBlocksCleared)
    {
        if (state.blocks.empty())
        {
            state.allBlocksCleared = true;
            TraceLog(LOG_INFO, "UpdateGame: All blocks cleared after music finished.");
        }
    }

    // 觸發遊戲結束流程
    if (state.musicFinished && state.allBlocksCleared && !state.gameEnding)
    {
        state.gameEnding = true;
        state.gameEndTime = GetTime();
        TraceLog(LOG_INFO, "UpdateGame: Starting game ending sequence (fade out).");
        if (state.audio.isValid && state.audio.bgm.ctxData != NULL && IsMusicStreamPlaying(state.audio.bgm))
        {
            StopMusicStream(state.audio.bgm);
            TraceLog(LOG_INFO, "UpdateGame: Explicitly stopping music stream in game ending sequence.");
        }
    }
}

void CleanupGame(GameState &state)
{
    TraceLog(LOG_INFO, "CleanupGame: Called for GameState.");
    if (state.audio.isValid && state.audio.bgm.ctxData != NULL) // 🔴 增加 ctxData 檢查
    {
        TraceLog(LOG_INFO, "CleanupGame: Stopping and unloading music stream from GameState...");
        if (IsMusicStreamPlaying(state.audio.bgm))
        {
            StopMusicStream(state.audio.bgm);
        }
        UnloadMusicStream(state.audio.bgm);
        state.audio.isValid = false;
        state.audio.bgm = {0}; // 清零
        TraceLog(LOG_INFO, "CleanupGame: Music stream from GameState cleaned up.");
    }
    else
    {
        TraceLog(LOG_INFO, "CleanupGame: No valid audio in GameState to clean up or BGM context was null.");
    }
}