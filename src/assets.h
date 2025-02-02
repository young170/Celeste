#pragma once

#include "celeste_lib.h"

// Constants

// Structs
enum SpriteID
{
  SPRITE_DICE,

  SPITE_COUNT
};

struct Sprite
{
  IVec2 atlasOffset;
  IVec2 spriteSize;
};

// Functions
Sprite get_sprite(SpriteID spriteID)
{
  Sprite sprite = {};

  switch (spriteID)
  {
    case SPRITE_DICE:
    {
      sprite.atlasOffset = {0, 0};
      sprite.spriteSize = {16, 16};
    }
  }

  return sprite;
}