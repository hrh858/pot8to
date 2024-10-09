#pragma once
#include "specs.h"
#include <cstddef>
#include <cstdint>

namespace Platform {
struct Context;

struct Program {
  uint8_t buffer[POT8TO_PROGRAM_MEMORY];
  size_t size;
};
Program pick_and_load_program();

void render_display(
    Context &ctx,
    const uint8_t display[POT8TO_DISPLAY_HEIGHT][POT8TO_DISPLAY_WIDTH]);

void beep();

uint8_t rnd_8bits();

void except_unknown_inst();

void block_for_input();
} // namespace Platform
