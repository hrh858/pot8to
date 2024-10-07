#include "platform.h"
#include "platform_windows.cpp"
#include "pot8to.cpp"
#include <commdlg.h>
#include <stdio.h>
#include <windows.h>

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
  HWND hwnd = CreateWindowEx(0,                   // Optional window styles
                             CLASS_NAME,          // Window class
                             "Pot8to",            // Window title
                             WS_OVERLAPPEDWINDOW, // Window style
                             CW_USEDEFAULT,
                             CW_USEDEFAULT, // Position
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
  Platform::Context ctx = {hwnd};

  // Show the window
  ShowWindow(hwnd, nCmdShow);

  // Run around 660 instructions per second
  const double targetInstructionTime = 1.0 / 660;
  // Render the display 60 times per second
  const double targetFrameTime = 1.0 / 60.0;

  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);

  LARGE_INTEGER previousTimeInstruction;
  LARGE_INTEGER previousTimeFrame;
  QueryPerformanceCounter(&previousTimeInstruction);
  QueryPerformanceCounter(&previousTimeFrame);

  double accumulatedTimeInstruction = 0.0;
  double accumulatedTimeFrame = 0.0;

  MSG msg = {0};
  while (true) {
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        return 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    double lastInstructionElapsedTime =
        (currentTime.QuadPart - previousTimeInstruction.QuadPart) /
        (double)frequency.QuadPart;
    double lastFrameElapsedTime =
        (currentTime.QuadPart - previousTimeFrame.QuadPart) /
        (double)frequency.QuadPart;

    accumulatedTimeInstruction += lastInstructionElapsedTime;
    accumulatedTimeFrame += lastFrameElapsedTime;

    printf("Accumulated Time Frame: %f", accumulatedTimeFrame);

    if (accumulatedTimeInstruction >= targetInstructionTime) {
      Pot8to::tick(emu);
      accumulatedTimeInstruction -= targetInstructionTime;
      previousTimeInstruction = currentTime;
    }
    if (accumulatedTimeFrame >= targetFrameTime) {
      Pot8to::decrement_timers(emu);
      Platform::render_display(ctx, emu.display);
      accumulatedTimeFrame -= targetFrameTime;
      previousTimeFrame = currentTime;
    }
  }

  return 0;
}
