#include "celeste_lib.h"

#include "input.h"

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


int main()
{
  BumpAllocator transientStorage = make_bump_allocator(MB(50));
  platform_create_window(1200, 720, "Hello World!");
  input.screenSizeX = 1200;
  input.screenSizeY = 720;

  gl_init(&transientStorage);

  while (running)
  {
    platform_update_window();
    gl_render();

    platform_swap_buffers();
  }

  return 0;
}