#pragma once

#include "input.h"
#include "celeste_lib.h"
#include "render_interface.h"


// Globals
constexpr int tset = 5;

// Structs
struct GameState
{
  IVec2 playerPos;
};

// Functions
extern "C"
{
  EXPORT_FN void update_game(RenderData *renderDataIn, Input *inputIn);
}