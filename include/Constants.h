#ifndef CONSTANTS_H
#define CONSTANTS_H
#pragma once
// 遊戲參數
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int LANE_COUNT = 4;
const float LANE_WIDTH = SCREEN_WIDTH / (float)LANE_COUNT;
const float JUDGE_LINE_Y = SCREEN_HEIGHT - 100;
const float BLOCK_SPEED = 300.0f; // 下降速度（像素/秒）

const float BLOCK_HEIGHT = 50.0f; // 方塊高度，與 DrawBlocks 中的一致

// 判定時間窗口 (秒) - 以 block.targetTime 為中心
const float PERFECT_TIMING_WINDOW = 0.05f; // ±50ms for Perfect (總共 100ms 窗口)
const float GOOD_TIMING_WINDOW = 0.10f;  // ±100ms for Good (總共 200ms 窗口)
                                         // Bad/Miss 就是超出 Good 範圍或空揮

// 分數
const int SCORE_PERFECT = 200;
const int SCORE_GOOD = 100;
const int SCORE_MISS = -50;      // 方塊錯過的分數懲罰
const int SCORE_BAD_PRESS = -20; // 空揮或錯誤按鍵的懲罰

enum class HitRating {
    NONE,    // 無判定或已過期
    PERFECT,
    GOOD,
    MISS,    // 方塊錯過
    BAD      // 空揮或錯誤時機的按鍵
};

enum class Scene {
    Intro,
    MainMenu,
    SongSelect,
    Playing,
    ScoreScreen
};

#endif