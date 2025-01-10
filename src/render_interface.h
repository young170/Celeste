#pragma once

#include "assets.h"
#include "celeste_lib.h"

// Constants
constexpr int MAX_TRANSFORMS = 1000;

// Structs
struct Transform
{
  IVec2 atlasOffset;
  IVec2 spriteSize;
  Vec2 pos;
  Vec2 size;
};

struct RenderData
{
  int transformCount;
  Transform transforms[MAX_TRANSFORMS];
};

// Globals
static RenderData *renderData;

// functions
void draw_sprite(SpriteID spriteID, Vec2 pos, Vec2 size)
{
  Sprite sprite = get_sprite(spriteID);

  Transform transform = {};
  transform.pos = pos;
  transform.size = size;
  transform.atlasOffset = sprite.atlasOffset;
  transform.spriteSize = sprite.spriteSize;

  renderData->transforms[renderData->transformCount++] = transform;
}