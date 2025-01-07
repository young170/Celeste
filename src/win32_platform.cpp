
#include "platform.h"
#include "celeste_lib.h"


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "wglext.h"


static HWND window;
static PFNWGLSWAPINTERVALEXTPROC  wglSwapIntervalEXT_ptr;

// callback function that processes messages sent to a window
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg,
                                          WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;

  switch (msg)
  {
    case WM_CLOSE:
    {
      running = false;
      break;
    }
    
    default:
    {
      // default input
      result = DefWindowProcA(window, msg, wParam, lParam);
    }
  }

  return result;
}

bool platform_create_window(int width, int height, char *title)
{
  HINSTANCE hInstance = GetModuleHandleA(0); // handler

  WNDCLASSA wcA = {};
  // use DefWindowProcA for default window procdure message-callback function
  wcA.lpfnWndProc = windows_window_callback;
  wcA.hInstance = hInstance;
  wcA.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
  wcA.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcA.lpszClassName = title; // window class name: ID
  

  if (!RegisterClassA(&wcA))
  {
    return false;
  }

  // (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX): overlapped window
  int dwStyle = WS_OVERLAPPEDWINDOW;

  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
  {
    /**
     * extended window style
     * name registered with the RegisterClass[Ex] function: use wcA's ID
     * window name: likely the title bar
     * window style
     * horizontal position of the window
     * vertical position of the window
     * width of the window
     * height of the window
     * handle to the parent window: used for child/owned windows
     * handle to the menu: NULL if the class menu is to be used
     * handle to the instance of the module connected to the window: hInstance
     * additional data which may be used to create the window
     */
    window = CreateWindowExA(0, title, // wcA's ID
                                  "Window Title",
                                  dwStyle,
                                  100,
                                  100,
                                  width,
                                  height,
                                  NULL,
                                  NULL,
                                  hInstance,
                                  NULL);

    if (window == NULL)
    {
      SM_ASSERT(false, "Failed to create Windows window");
      return false;
    }

    HDC fakeDC = GetDC(window);
    if (!fakeDC)
    {
      SM_ASSERT(false, "Failed to get HDC");
      return false;
    }

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;

    int pixelFormat = ChoosePixelFormat(fakeDC, &pfd);
    if (!pixelFormat)
    {
      SM_ASSERT(false, "Failed to choose pixel format");
      return false;
    }

    if (!SetPixelFormat(fakeDC, pixelFormat, &pfd))
    {
      SM_ASSERT(false, "Failed to set pixel format");
      return false;
    }

    HGLRC fakeRC = wglCreateContext(fakeDC);
    if (!fakeRC)
    {
      SM_ASSERT(false, "Failed to crete render context");
      return false;
    }

    if (!wglMakeCurrent(fakeDC, fakeRC))
    {
      SM_ASSERT(false, "Failed to make current");
      return false;
    }

    // set attributes: opengl ver. window pixel format
    wglChoosePixelFormatARB = 
      (PFNWGLCHOOSEPIXELFORMATARBPROC)platform_load_gl_function("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = 
      (PFNWGLCREATECONTEXTATTRIBSARBPROC)platform_load_gl_function("wglCreateContextAttribsARB");
    wglSwapIntervalEXT_ptr =
      (PFNWGLSWAPINTERVALEXTPROC)platform_load_gl_function("wglSwapIntervalEXT");

    if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)
    {
      SM_ASSERT(false, "Failed to load OpenGL functions");
      return false;
    }

    // clean up
    wglMakeCurrent(fakeDC, 0);
    wglDeleteContext(fakeRC);
    ReleaseDC(window, fakeDC);

    // destroy window
    DestroyWindow(window);
  }

  // Actual OpenGL initialization
  {
    // Add in the border size of the window
    {
      RECT borderRect = {};
      AdjustWindowRectEx(&borderRect, dwStyle, 0, 0);

      width += borderRect.right - borderRect.left;
      height += borderRect.bottom - borderRect.top;
    }

    window = CreateWindowExA(0, title, // This references lpszClassName from wc
                            title,    // This is the actual Title
                            dwStyle,
                            100,
                            100,
                            width,
                            height,
                            NULL,     // parent
                            NULL,     // menu
                            hInstance,
                            NULL);    // lpParam

    if(window == NULL)
    {
      SM_ASSERT(false, "Failed to create Windows Window");
      return false;
    }

    HDC dc = GetDC(window);
    if(!dc)
    {
      SM_ASSERT(false, "Failed to get DC");
      return false;
    }

    const int pixelAttribs[] =
    {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
      WGL_SWAP_METHOD_ARB,    WGL_SWAP_COPY_ARB,
      WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
      WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
      WGL_COLOR_BITS_ARB,     32,
      WGL_ALPHA_BITS_ARB,     8,
      WGL_DEPTH_BITS_ARB,     24,
      0 // Terminate with 0, otherwise OpenGL will throw an Error!
    };

    UINT numPixelFormats;
    int pixelFormat = 0;
    if(!wglChoosePixelFormatARB(dc, pixelAttribs,
                                0, // Float List
                                1, // Max Formats
                                &pixelFormat,
                                &numPixelFormats))
    {
      SM_ASSERT(0, "Failed to wglChoosePixelFormatARB");
      return false;
    }

    PIXELFORMATDESCRIPTOR pfd = {0};
    DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if(!SetPixelFormat(dc, pixelFormat, &pfd))
    {
      SM_ASSERT(0, "Failed to SetPixelFormat");
      return true;
    }

    // define OpenGL ver. attributes
    const int contextAttribs[] =
    {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 3,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
      0 // Terminate the Array
    };

    HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
    if(!rc)
    {
      SM_ASSERT(0, "Failed to crate Render Context for OpenGL");
      return false;
    }

    if(!wglMakeCurrent(dc, rc))
    {
      SM_ASSERT(0, "Faield to wglMakeCurrent");
      return false;
    }
  }

  // (window_handler, control_how_to_show)
  ShowWindow(window, SW_SHOW); // created window, activates window and displays in its current dimensions

  return true;
}

/**
 * Given a queue of messages, peek at the top then dispatch the top message
 */
void platform_update_window()
{
  MSG msg;

  // PM_REMOVE: removes message after processing by PeekMessage
  while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg); // virtual-key msg -> char msg: post to message queue to be read by PeekMessage()
    DispatchMessage(&msg); // dispatches a message to a window procedure: in this case HWND window
  }
}

void *platform_load_gl_function(char *funName)
{
  PROC proc = wglGetProcAddress(funName);
  if (!proc)
  {
    static HMODULE openglDLL = LoadLibraryA("opengl32.dll");
    proc = GetProcAddress(openglDLL, funName);
    if (!proc)
    {
      SM_ASSERT(false, "Failed to load gl function %s", funName);
      return nullptr;
    }
  }

  return (void *)proc;
}