#ifndef RENDER_H
#define RENDER_H

#include "Block.h"
#include "Input.h"
#include <vector>
#include "Constants.h"
#include "Game.h"

struct GameState; // Forward declaration of GameState

#pragma once

void DrawGame(const GameState& gameState, const InputState& inputState);
void DrawIntroScene(GameState& state);
void DrawMainMenu(GameState& state);
void DrawSongSelect(GameState& state);
void DrawScoreScreen(const GameState& state); // 🆕 聲明繪製成績畫面的函數

// 🔵 新增背景資源加載與釋放函式
void LoadGameAssets();
void UnloadGameAssets();

#endif