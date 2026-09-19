#include "Block.h"
#include "Constants.h"
#include "Game.h"
#include <raylib.h>
#include <cstdlib>
#include <algorithm>

void SpawnBlock(std::vector<Block> &blocks,
    double &nextSpawnTime,
    float spawnInterval,
    float offset,
    double musicStartTime)
{
double currentTime = GetTime() - musicStartTime;
float travelTime = JUDGE_LINE_Y / BLOCK_SPEED;  // 幾秒後方塊剛好到判定線

// 當 currentTime 到達 spawnTime，就生成方塊
while (true)
{
double targetTime = nextSpawnTime + offset;           // 方塊應該抵達判定線的時間
double spawnTime  = targetTime - travelTime;          // 方塊應該出現在 y=0 的時間

if (currentTime >= spawnTime)
{
Block block;
block.targetTime = targetTime;
block.startTime  = spawnTime;  // 後面用來算 y = (now–startTime)*speed
block.lane       = rand() % LANE_COUNT;
block.active     = true;
blocks.push_back(block);

TraceLog(LOG_INFO, "Spawned block: start=%.3f, target=%.3f, lane=%d",
         block.startTime, block.targetTime, block.lane);

nextSpawnTime += spawnInterval;  // 下一拍
}
else break;
}
}

void UpdateBlocks(std::vector<Block>& blocks, GameState& gameState, double musicStartTime) {
    double currentTime = GetTime() - musicStartTime;

    for (auto it = blocks.begin(); it != blocks.end(); /* no increment in loop header */) {
        Block& block = *it; // Use reference

        if (!block.active) { // 如果方塊已被擊中或因其他原因失效
            ++it;
            continue;
        }

        // 更新 Y 座標
        double elapsedSinceBlockSpawn = currentTime - block.startTime;
        block.y = elapsedSinceBlockSpawn * BLOCK_SPEED;

        // 判斷 Miss
        // 如果方塊的 targetTime 已經過去了 GOOD_TIMING_WINDOW 這麼久，
        // 並且方塊仍然是 active (意味著沒有被成功擊中)
        if (block.active && (currentTime > block.targetTime + GOOD_TIMING_WINDOW)) {
            TraceLog(LOG_INFO, "Block MISSED. Lane: %d, TargetTime: %.3f, CurrentTime: %.3f",
                     block.lane, block.targetTime, currentTime);
            block.active = false; // 標記為 inactive，因為它 Miss 了
            
            gameState.score += SCORE_MISS; // 更新分數
            gameState.lastHitRating = HitRating::MISS; // 設置判定結果
            gameState.hitRatingDisplayTime = GameState::HIT_RATING_DURATION; // 顯示提示
            
            // Miss 後，方塊變為 inactive，會被後面的 erase-remove 清理
        }

        // 如果方塊完全移出屏幕底部，也標記為 inactive
        // 這個條件可以作為一個備份，確保不會有方塊無限存在
        // BLOCK_HEIGHT 應從 Constants.h 獲取
        if (block.y > SCREEN_HEIGHT + BLOCK_HEIGHT) { // 完全移出屏幕
            if (block.active) { // 如果是因為超出屏幕而失效，且之前未被判 Miss
                TraceLog(LOG_DEBUG, "Block out of screen (and was active). Lane: %d. Marking inactive.", block.lane);
                // 這裡不應該再算 Miss，因為 Miss 判斷應該在 JUDGE_LINE 附近發生
                // 如果到這裡還是 active，說明錯過了 Miss 判斷，或者 Miss 判斷邏輯有問題
                // 為了安全，只標記 inactive
            }
            block.active = false;
        }

        // 迭代器處理：只有當前元素未被標記為 inactive（即未被 erase）時才遞增
        // 但由於我們是統一在循環後 erase，所以這裡總是遞增
        ++it;
    }

    // 清除所有 inactive 的方塊 (包括被擊中的、Miss的、超出屏幕的)
    blocks.erase(
        std::remove_if(blocks.begin(), blocks.end(),
                       [](const Block& b){ return !b.active; }),
        blocks.end()
    );
}

void DrawBlocks(const std::vector<Block>& blocks) // 🔴 確保函數頭與聲明完全一致
{
    for (const auto& block : blocks)
    {
        // 只畫 y>=0 的方塊
        if (block.active && block.y >= 0)
        {
            // 根據 Constants.h 中的定義來計算 x 和寬度
            // float x = block.lane * LANE_WIDTH + LANE_WIDTH / 4; // 舊的計算方式
            // DrawRectangle(x, block.y, LANE_WIDTH / 2, 50, WHITE);

            // 使用 Constants.h 中的 BLOCK_HEIGHT
            float rectX = block.lane * LANE_WIDTH + (LANE_WIDTH - (LANE_WIDTH / 1.5f)) / 2.0f; // 居中方塊
            float rectWidth = LANE_WIDTH / 1.5f; // 方塊寬度可以調整
            
            // 使用 Constants.h 中的 BLOCK_HEIGHT (如果定義了)
            // 否則使用你之前繪製時的固定值 50
            #ifdef BLOCK_HEIGHT
                DrawRectangleRec(Rectangle{rectX, block.y, rectWidth, BLOCK_HEIGHT}, WHITE);
            #else
                DrawRectangleRec(Rectangle{rectX, block.y, rectWidth, 50.0f}, WHITE); // 使用之前的值 50
            #endif
        }
    }
}