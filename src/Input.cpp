#include "Input.h"
#include "Game.h"       // For GameState access
#include "Constants.h"  // For timing windows, scores, HitRating enum
#include <raylib.h>
#include <cmath>        // For std::abs
#include <algorithm>    // Potentially for std::min_element, though not used in current logic

void InitializeInput(InputState& state) {
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "Audio device not ready in InitializeInput. Attempting init.");
        InitAudioDevice();
    }
    state.hitSound = LoadSound("resources/hit.wav"); // Generic hit sound
    // Example for specific sounds:
    // state.perfectSound = LoadSound("resources/perfect.wav");
    // state.goodSound = LoadSound("resources/good.wav");
    // state.missSound = LoadSound("resources/miss.wav");

    if (state.hitSound.frameCount == 0) {
         TraceLog(LOG_WARNING, "Failed to load sound: resources/hit.wav");
    }
}

void HandleInput(InputState& inputState, GameState& gameState) {
    // 🔴 日誌 1: 確認 HandleInput 是否被調用以及初始狀態
    TraceLog(LOG_INFO, "[HandleInput ENTRY] Called. Countdown:%s, GameEnding:%s, AudioValid:%s",
             gameState.isCountdown ? "T":"F",
             gameState.gameEnding ? "T":"F",
             gameState.audio.isValid ? "T":"F");

    if (gameState.isCountdown || gameState.gameEnding || !gameState.audio.isValid) {
        TraceLog(LOG_INFO, "[HandleInput EXIT] Condition not met (Countdown/GameEnding/AudioInvalid)."); // 🔴 日誌
        return;
    }

    double currentTime = GetTime() - gameState.musicStartTime;
    // 🔴 日誌 2: 打印計算出的 currentTime
    TraceLog(LOG_INFO, "[HandleInput INFO] CurrentTime relative to music start: %.3f", currentTime);

    auto processLaneHit = [&](int lane) {
        // 🔴 日誌 3: 確認 processLaneHit 被哪個按鍵觸發
        TraceLog(LOG_INFO, "[processLaneHit ENTRY] Lane: %d triggered.", lane);

        Block* bestBlockToHit = nullptr;
        float smallestTimeDiffToTarget = -1.0f; // -1.0f or a large float for not found

        // 1. 找出最佳方塊
        for (auto& block : gameState.blocks) {
            if (block.active && block.lane == lane) {
                float searchWindow = GOOD_TIMING_WINDOW * 1.5f; // 搜索窗口
                float timeToTarget = block.targetTime - currentTime; // 有符號的時間差
                float absTimeToTarget = std::abs(timeToTarget);

                // 🔴 日誌 4: 打印每個被檢查的方塊的信息
                TraceLog(LOG_DEBUG, "[processLaneHit CHECKING_BLOCK] Lane:%d, BlockY:%.1f, Active:%s, TargetTime:%.3f, AbsDiffToCurrent:%.3f (SearchWin:%.3f)",
                         block.lane, block.y, block.active ? "T":"F", block.targetTime, absTimeToTarget, searchWindow);

                if (absTimeToTarget <= searchWindow) {
                    if (bestBlockToHit == nullptr || absTimeToTarget < smallestTimeDiffToTarget) {
                        smallestTimeDiffToTarget = absTimeToTarget;
                        bestBlockToHit = &block;
                        // 🔴 日誌 5: 打印找到潛在最佳方塊的信息
                        TraceLog(LOG_INFO, "[processLaneHit NEW_BEST_BLOCK] Lane:%d, TargetTime:%.3f, AbsDiff:%.3f (smallest so far)",
                                 lane, bestBlockToHit->targetTime, smallestTimeDiffToTarget);
                    }
                }
            }
        }

        if (bestBlockToHit) {
            // 🔴 日誌 6: 確認找到了 bestBlockToHit 並打印其信息
            TraceLog(LOG_INFO, "[processLaneHit BEST_BLOCK_FOUND] Lane:%d, TargetTime:%.3f, Final AbsDiff:%.3f. Comparing with PERFECT(%.3f) & GOOD(%.3f) windows.",
                     lane, bestBlockToHit->targetTime, smallestTimeDiffToTarget, PERFECT_TIMING_WINDOW, GOOD_TIMING_WINDOW);

            if (smallestTimeDiffToTarget <= PERFECT_TIMING_WINDOW) {
                gameState.score += SCORE_PERFECT;
                gameState.lastHitRating = HitRating::PERFECT;
                if (inputState.hitSound.frameCount > 0) PlaySound(inputState.hitSound);
                bestBlockToHit->active = false;
                TraceLog(LOG_INFO, "HIT: PERFECT! Lane: %d", lane); // 簡化日誌
            } else if (smallestTimeDiffToTarget <= GOOD_TIMING_WINDOW) {
                gameState.score += SCORE_GOOD;
                gameState.lastHitRating = HitRating::GOOD;
                if (inputState.hitSound.frameCount > 0) PlaySound(inputState.hitSound);
                bestBlockToHit->active = false;
                TraceLog(LOG_INFO, "HIT: GOOD. Lane: %d", lane); // 簡化日誌
            } else {
                gameState.score += SCORE_BAD_PRESS;
                gameState.lastHitRating = HitRating::BAD;
                TraceLog(LOG_INFO, "HIT: BAD TIMING (for block). Lane: %d. Block not consumed.", lane); // 簡化日誌
            }
            gameState.hitRatingDisplayTime = GameState::HIT_RATING_DURATION;
        } else {
            // 🔴 日誌 7: 確認沒有找到可擊中的方塊
            TraceLog(LOG_INFO, "[processLaneHit NO_BEST_BLOCK] Lane:%d. Empty press or no block in window.", lane);
            gameState.score += SCORE_BAD_PRESS;
            gameState.lastHitRating = HitRating::BAD;
            gameState.hitRatingDisplayTime = GameState::HIT_RATING_DURATION;
        }
        TraceLog(LOG_INFO, "[processLaneHit EXIT] Lane: %d finished processing.", lane); // 🔴 日誌
    }; // End of processLaneHit lambda

    if (IsKeyPressed(KEY_A)) processLaneHit(0);
    else if (IsKeyPressed(KEY_S)) processLaneHit(1);
    else if (IsKeyPressed(KEY_D)) processLaneHit(2);
    else if (IsKeyPressed(KEY_F)) processLaneHit(3);

    if (gameState.hitRatingDisplayTime > 0) {
        gameState.hitRatingDisplayTime -= GetFrameTime();
        if (gameState.hitRatingDisplayTime < 0) {
            gameState.lastHitRating = HitRating::NONE;
            gameState.hitRatingDisplayTime = 0;
        }
    }
    TraceLog(LOG_INFO, "[HandleInput EXIT] Finished."); // 🔴 日誌
}