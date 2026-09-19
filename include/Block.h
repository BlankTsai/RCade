#ifndef BLOCK_H
#define BLOCK_H

#include <raylib.h>
#include <vector>
#include "Constants.h"

struct GameState; // Forward declaration

struct Block {
    float startTime;  // 方塊開始下降的時間（秒，相對音樂開始）
    float targetTime; // 應到達判定線的時間（秒，相對音樂開始）
    int lane;         // 方塊所在軌道
    bool active;      // 是否有效
    float y;          // 當前 Y 座標，初始應為 0
};

void SpawnBlock(std::vector<Block>& blocks, double& nextSpawnTime, float spawnInterval, float offset, double musicStartTime);
void UpdateBlocks(std::vector<Block>& blocks, GameState& gameState, double musicStartTime); // 新的
void DrawBlocks(const std::vector<Block>& blocks);

#endif