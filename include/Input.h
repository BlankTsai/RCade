#ifndef INPUT_H
#define INPUT_H

#include "Block.h"
#include <vector>
#include "Constants.h"
#include <raylib.h>    // For Sound

// Forward declaration of GameState is crucial here
struct GameState; // 🟢 前向聲明 GameState

struct InputState 
{
    Sound hitSound;
};

void InitializeInput(InputState& state);
void HandleInput(InputState& inputState, GameState& gameState);

#endif