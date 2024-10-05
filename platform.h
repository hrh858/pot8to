#pragma once
#include "specs.h"
#include <cstddef>
#include <cstdint>

namespace Platform {
// The emulator provides to the OS.
// give timing, input get bitman, sound buffer
void render_and_play_sound(
    const uint8_t screen[POT8TO_DISPLAY_HEIGHT][POT8TO_DISPLAY_WIDTH],
    bool sound);

struct Program {
  uint8_t buffer[POT8TO_PROGRAM_MEMORY];
  size_t size;
};
Program pick_and_load_program();

uint8_t rnd_8bits();

void except_unknown_inst();

void block_for_input();
} // namespace Platform
