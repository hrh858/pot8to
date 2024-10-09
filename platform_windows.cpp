#pragma once
#include "platform.h"
#include <windows.h>

namespace Platform {
struct Context {
  HWND &hwnd;
};

Program pick_and_load_program() {
  Program program = {};

  OPENFILENAMEW ofn;              // Common dialog box structure
  wchar_t szFile[MAX_PATH] = {0}; // Buffer for file name

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
  ofn.lpstrFilter = L"Chip8 ROMs\0*.ch8\0";
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  ofn.lpstrTitle = L"Open a Chip8 ROM";

  // Display the Open dialog box
  if (GetOpenFileNameW(&ofn) == TRUE) {
    LPWSTR filepath = ofn.lpstrFile;
    HANDLE fileHandle =
        CreateFileW(filepath, GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fileHandle == INVALID_HANDLE_VALUE) {
      // TODO: Handle this possible error
    }

    DWORD fileSize = GetFileSize(fileHandle, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize > POT8TO_PROGRAM_MEMORY) {
      // TODO: Handle ths possible error
    }

    program.size = fileSize;
    BOOL readSuccessful =
        ReadFile(fileHandle, &program.buffer, fileSize, NULL, NULL);

    if (!readSuccessful) {
      // TODO: Handle this possible error
    }

    CloseHandle(fileHandle);
  } else {
  }

  return program;
}

// NOTE: This funciton in Windows may need to receive `hwnd`...
void render_display(
    Context &ctx,
    const uint8_t display[POT8TO_DISPLAY_HEIGHT][POT8TO_DISPLAY_WIDTH]) {
  // Get device context of the window
  HDC hdc = GetDC(ctx.hwnd);

  // Define the size of each CHIP-8 pixel to be drawn
  const int pixelSize = 15; // Adjusted size to match window setup

  // Loop through the CHIP-8 display array
  for (int y = 0; y < POT8TO_DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < POT8TO_DISPLAY_WIDTH; x++) {
      // Determine color based on display value
      HBRUSH brush;
      RECT rect = {x * pixelSize, y * pixelSize, (x + 1) * pixelSize,
                   (y + 1) * pixelSize};
      if (display[y][x] == 1) {
        // Set brush to white to draw a filled rectangle
        brush = CreateSolidBrush(RGB(255, 255, 255));
      } else {
        // Clear the pixel if it should be empty
        brush = CreateSolidBrush(RGB(0, 0, 0));
      }
      FillRect(hdc, &rect, brush);
      DeleteObject(brush);
    }
  }

  // Release the device context
  ReleaseDC(ctx.hwnd, hdc);
}

void beep() { Beep(1000, 100); }

uint8_t rnd_8bits() {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (uint8_t)(counter.QuadPart & 0xFF);
}

void block_for_input() {}

void except_unknown_inst() {
  // TODO: Implement - This is called by the emulator when it finds an
  // unknown instruction (which should not happen in a sane program).
}
} // namespace Platform
