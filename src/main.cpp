#include "celeste_lib.h"

#include "platform.h"

#define APIENTRY
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

// must be lower than *_platform.cpp
// platform_load_gl_function() is undefined
#include "gl_renderer.h" 


int main()
{
  platform_create_window(1200, 720, "Hello World!");

  while (running)
  {
    platform_update_window();
  }

  return 0;
}