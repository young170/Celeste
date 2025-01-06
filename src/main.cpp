#include "celeste_lib.h"

static bool running = true;


bool platform_create_window(int width, int height, char *title);
void platform_update_window();


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>


static HWND window;


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
    return false;
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


#endif


int main()
{
  platform_create_window(1200, 720, "Hello World!");

  while (running)
  {
    platform_update_window();

    SM_TRACE("Test");
    SM_ASSERT(false, "assert");
  }

  return 0;
}