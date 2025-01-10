#include "celeste_lib.h"

#include "input.h"

#include "game.h"

#include "platform.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

// must be lower than *_platform.cpp
// platform_load_gl_function() is undefined
#include "gl_renderer.cpp" 


typedef decltype(update_game) update_game_type;
static update_game_type *update_game_ptr;

void reload_game_dll(BumpAllocator *transientStorage);

int main()
{
  BumpAllocator transientStorage = make_bump_allocator(MB(50));
  BumpAllocator persistentStorage = make_bump_allocator(MB(50));

  input = (Input *)bump_alloc(&persistentStorage, sizeof(Input));
  if (!input)
  {
    SM_ERROR("Failed to allocate input");
    return -1;
  }
  renderData = (RenderData *)bump_alloc(&persistentStorage, sizeof(RenderData));
  if (!renderData)
  {
    SM_ERROR("Failed to allocate renderData");
    return -1;
  }

  platform_create_window(1200, 720, "Hello World!");
  input->screenSizeX = 1200;
  input->screenSizeY = 720;

  gl_init(&transientStorage);

  while (running)
  {
    reload_game_dll(&transientStorage);

    // updating
    platform_update_window();
    update_game(renderData, input);
    gl_render();

    platform_swap_buffers();

    transientStorage.used = 0;
  }

  return 0;
}

void update_game(RenderData *renderDataIn, Input *inputIn)
{
  if (update_game_ptr)
  {
    update_game_ptr(renderDataIn, inputIn);
  }
  else
  {
    SM_ERROR("update_game_ptr not initialized");
  }
}

void reload_game_dll(BumpAllocator *transientStorage)
{
  static void *gameDLL;
  static long long lastEditTimestempGameDLL;

  long long currentTimestempGameDLL = get_timestamp("game.dll");
  if (currentTimestempGameDLL > lastEditTimestempGameDLL)
  {
    if (gameDLL)
    {
      bool freeResult = platform_free_dynamic_library(gameDLL);
      SM_ASSERT(freeResult, "Failed to load free game.dll");
      gameDLL = nullptr;
      SM_TRACE("Freed game.dll");
    }

    while (!copy_file("game.dll", "game_load.dll", transientStorage))
    {
      Sleep(10);
    }
    SM_TRACE("Copied game.dll into game_load.dll");

    update_game_ptr = (update_game_type *)platform_load_dynamic_function(gameDLL, "update_game");
    SM_ASSERT(update_game_ptr, "Failed to load update_game function");
    lastEditTimestempGameDLL = currentTimestempGameDLL;
  }
}