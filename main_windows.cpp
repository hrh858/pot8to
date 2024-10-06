#include "platform.h"
#include "pot8to.cpp"
#include <Windows.h>
#include <commdlg.h>
#include <stdio.h>

// Window Procedure function to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  Platform::Context ctx = {};
  Platform::Program rom = Platform::pick_and_load_program();
  if (rom.size < 0) {
    // TODO: Finish execution
  }
  Pot8to::State emu = Pot8to::initialize(rom);

  const int pixelSize = 15;
  // Define the window class
  const char CLASS_NAME[] = "Pot8to";

  WNDCLASS wc = {};
  wc.lpfnWndProc = WindowProc;   // Set the window procedure
  wc.hInstance = hInstance;      // Handle to application instance
  wc.lpszClassName = CLASS_NAME; // Window class name

  // Register the window class
  RegisterClass(&wc);

  // Create the window
  HWND hwnd = CreateWindowEx(
    0,                   // Optional window styles
    CLASS_NAME,          // Window class
    "Pot8to",            // Window title
    WS_OVERLAPPEDWINDOW, // Window style
    CW_USEDEFAULT,
    CW_USEDEFAULT,       // Position
    64 * pixelSize,
    32 * pixelSize, // Size
    NULL,           // Parent window
    NULL,           // Menu
    hInstance,      // Instance handle
    NULL            // Additional application data
  );

  if (hwnd == NULL) {
    return 0; // Handle window creation failure
  }

  // Show the window
  ShowWindow(hwnd, nCmdShow);

  // Run the message loop
  const double targetFrameTime = 1.0 / 60.0;

  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  LARGE_INTEGER previousTime;
  QueryPerformanceCounter(&previousTime);
  double accumulatedTime = 0.0;

  while (true) {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    double elapsedTime =
        (currentTime.QuadPart - previousTime.QuadPart) / frequency.QuadPart;
    accumulatedTime += elapsedTime;

    while (accumulatedTime >= targetFrameTime) {
      Pot8to::tick(emu);
      // Pot8to::update_timers(emu);
      accumulatedTime -= targetFrameTime;
    }

    Platform::render_display(ctx, emu.display);
  }

  return 0;
}
