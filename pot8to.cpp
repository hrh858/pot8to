// #include "platform.h"
#include "specs.h"
#include <cstddef>
#include <cstdint>

namespace Pot8to {
struct State {
  // Add the default sprites here
  uint8_t memory[POT8TO_MAX_MEMORY] = {};
  bool keyboard[16] = {};
  uint16_t stack[16] = {};
  uint8_t display[POT8TO_DISPLAY_HEIGHT][POT8TO_DISPLAY_WIDTH] = {};
  struct {
    // General purpose registers
    uint8_t V[16] = {};
    // Instruction pointer
    uint16_t I = 0;
    // Timers
    struct {
      uint8_t sound = 0;
      uint8_t delay = 0;
    } T;
    // Program counter - Always start at 0x200!
    uint16_t PC = POT8TO_PROGRAM_MEMORY_INITIAL_POSITION;
    // Stack pointer
    uint8_t SP = 0;
  } registers;
};

static void load_rom(State &state, Platform::Program &program) {
  if (program.size > POT8TO_PROGRAM_MEMORY) {
    return;
  }
  for (size_t i = 0; i < program.size; i++) {
    state.memory[POT8TO_PROGRAM_MEMORY_INITIAL_POSITION + i] =
        program.buffer[i];
  }
}

State initialize(Platform::Program &program) {
  State s = State{};

  uint8_t default_sprites[16][5] = {
      {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
      {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
      {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
      {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
      {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
      {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
      {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
      {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
      {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
      {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
      {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
      {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
      {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
      {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
      {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
      {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
  };

  // I guess the compiler will optimize this
  // and if it doesn't who cares lol
  for (size_t i = 0; i < 16; i++) {
    for (size_t j = 0; j < 5; j++) {
      s.memory[i * 5 + j] = default_sprites[i][j];
    }
  }

  load_rom(s, program);

  return s;
}

enum InstructionIdentifier {
  INST_00E0,
  INST_00EE,
  INST_1NNN,
  INST_2NNN,
  INST_3XNN,
  INST_4XNN,
  INST_5XY0,
  INST_6XNN,
  INST_7XNN,
  INST_8XY0,
  INST_8XY1,
  INST_8XY2,
  INST_8XY3,
  INST_8XY4,
  INST_8XY5,
  INST_8XY6,
  INST_8XY7,
  INST_8XYE,
  INST_9XY0,
  INST_ANNN,
  INST_BNNN,
  INST_CXNN,
  INST_DXYN,
  INST_EX9E,
  INST_EXA1,
  INST_FX07,
  INST_FX0A,
  INST_FX15,
  INST_FX18,
  INST_FX1E,
  INST_FX29,
  INST_FX33,
  INST_FX55,
  INST_FX65,
  INST_UNKNOWN
};

struct Instruction {
  InstructionIdentifier identifier;
  struct {
    uint8_t vx = 0;
    uint8_t vy = 0;
  } registers;
  union {
    uint8_t N = 0;
    uint8_t NN;
    uint16_t NNN;
  } address;
};

static Instruction decode_next_intruction(State &state) {
  uint16_t inst_raw = (state.memory[state.registers.PC] << 8) |
                      state.memory[state.registers.PC + 1];

  Instruction inst = {};
  switch (inst_raw & 0xF000) {
  case 0x0000:
    switch (inst_raw) {
    case 0x00E0: // 00E0
      inst.identifier = INST_00E0;
      break;
    case 0x00EE: // 00EE
      inst.identifier = INST_00EE;
      break;
    }
    break;
  case 0x1000: // 1NNN
    inst.identifier = INST_1NNN;
    inst.address.NNN = inst_raw & 0xFFF;
    break;
  case 0x2000: // 2NNN
    inst.identifier = INST_2NNN;
    inst.address.NNN = inst_raw & 0x0FFF;
    break;
  case 0x3000: // 3XNN
    inst.identifier = INST_3XNN;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.address.NN = inst_raw & 0x00FF;
    break;
  case 0x4000: // 4XNN
    inst.identifier = INST_4XNN;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.address.NN = inst_raw & 0x00FF;
    break;
  case 0x5000: // 5XY0
    inst.identifier = INST_5XY0;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.registers.vy = (inst_raw & 0x00F0) >> 4;
    break;
  case 0x6000: // 6XNN
    inst.identifier = INST_6XNN;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.address.NN = inst_raw & 0x00FF;
    break;
  case 0x7000: // 7XNN
    inst.identifier = INST_7XNN;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.address.NN = inst_raw & 0x00FF;
    break;
  case 0x8000:
    switch (inst_raw & 0x00F) {
    case 0x0000: // 8XY0
      inst.identifier = INST_8XY0;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0001: // 8XY1
      inst.identifier = INST_8XY1;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0002: // 8XY2
      inst.identifier = INST_8XY2;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0003: // 8XY3
      inst.identifier = INST_8XY3;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0004: // 8XY4
      inst.identifier = INST_8XY4;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0005: // 8XY5
      inst.identifier = INST_8XY5;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0006: // 8XY6
      inst.identifier = INST_8XY6;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x0007: // 8XY7
      inst.identifier = INST_8XY7;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    case 0x000E: // 8XYE
      inst.identifier = INST_8XYE;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      inst.registers.vy = (inst_raw & 0x00F0) >> 4;
      break;
    }
    break;
  case 0x9000: // 9XY0
    inst.identifier = INST_9XY0;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.registers.vy = (inst_raw & 0x00F0) >> 4;
    break;
  case 0xA000: // ANNN
    inst.identifier = INST_ANNN;
    inst.address.NNN = inst_raw & 0x0FFF;
    break;
  case 0xB000: // BNNN
    inst.identifier = INST_BNNN;
    inst.address.NNN = inst_raw & 0x0FFF;
    break;
  case 0xC000: // CXNN
    inst.identifier = INST_CXNN;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.address.NN = inst_raw & 0x00FF;
    break;
  case 0xD000: // DXYN
    inst.identifier = INST_DXYN;
    inst.registers.vx = (inst_raw & 0x0F00) >> 8;
    inst.registers.vy = (inst_raw & 0x00F0) >> 4;
    inst.address.N = inst_raw & 0x000F;
    break;
  case 0xE000:
    switch (inst_raw & 0x00FF) {
    case 0x009E: // EX9E
      inst.identifier = INST_EX9E;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x00A1: // EXA1
      inst.identifier = INST_EXA1;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    }
    break;
  case 0xF000:
    switch (inst_raw & 0x00FF) {
    case 0x0007: // FX07
      inst.identifier = INST_FX07;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x000A: // FX0A
      inst.identifier = INST_FX0A;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x0015: // FX15
      inst.identifier = INST_FX15;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x0018: // FX18
      inst.identifier = INST_FX18;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x001E: // FX1E
      inst.identifier = INST_FX1E;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x0029: // FX29
      inst.identifier = INST_FX29;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x0033: // FX33
      inst.identifier = INST_FX33;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x0055: // FX55
      inst.identifier = INST_FX55;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    case 0x0065: // FX65
      inst.identifier = INST_FX65;
      inst.registers.vx = (inst_raw & 0x0F00) >> 8;
      break;
    }
    break;
  default:
    inst.identifier = INST_UNKNOWN;
    break;
  };

  state.registers.PC += 2;
  return inst;
}

static void execute_decoded_instruction(State &state,
                                        const Instruction &instruction) {
  switch (instruction.identifier) {
  case INST_00E0:
    for (size_t i = 0; i < POT8TO_DISPLAY_HEIGHT; i++) {
      for (size_t j = 0; j < POT8TO_DISPLAY_WIDTH; j++) {
        state.display[i][j] = 0;
      }
    }
    break;

  case INST_2NNN:
    state.stack[state.registers.SP] = state.registers.PC;
    state.registers.SP++;
    state.registers.PC = instruction.address.NNN;
    break;

  case INST_6XNN:
    state.registers.V[instruction.registers.vx] = instruction.address.NN;
    break;

  case INST_8XY7: {
    int16_t sub = (int16_t)state.registers.V[instruction.registers.vy] -
                  (int16_t)state.registers.V[instruction.registers.vx];
    uint8_t carry_flag = sub >= 0;
    state.registers.V[instruction.registers.vx] = (uint8_t)(sub & 0xFF);
    state.registers.V[0xF] = carry_flag;
  } break;

  case INST_BNNN:
    state.registers.PC = instruction.address.NNN + state.registers.V[0];
    break;

  case INST_CXNN: {
    // We need a way to generate a random number, so let's use the platform
    // for now.
    uint8_t rnd = Platform::rnd_8bits();
    state.registers.V[instruction.registers.vx] = rnd & instruction.address.NN;
  } break;

  case INST_DXYN: {
    // state: Current state of the emulator - CPU registers, display, memory...
    // instruction: Data decoded from the currently executing instruction.
    bool collision = false;
    for (size_t i = 0; i < instruction.address.N; i++) {
      // Extract the value of each bit in the byte and interpret it as a pixel.
      uint8_t byte = state.memory[state.registers.I + i];
      size_t row = (state.registers.V[instruction.registers.vy] + i) %
                   POT8TO_DISPLAY_HEIGHT;
      for (uint8_t j = 0; j < 8; j++) {
        // Draw the pixel.
        uint8_t pixel = (byte >> (7 - j)) & 0b1;
        size_t col = (state.registers.V[instruction.registers.vx] + j) %
                     POT8TO_DISPLAY_WIDTH;
        // Check for collision.
        collision = collision || ((state.display[row][col] & pixel) == 1);
        state.display[row][col] ^= pixel;
      }
    }
    // Set VF if collision.
    state.registers.V[0xF] = collision ? 1 : 0;
  } break;

  case INST_3XNN:
    state.registers.PC += 2 * (state.registers.V[instruction.registers.vx] ==
                               instruction.address.NN);
    break;

  case INST_00EE:
    state.registers.SP--;
    state.registers.PC = state.stack[state.registers.SP];
    break;

  case INST_8XY5: {
    int16_t sub = (int16_t)state.registers.V[instruction.registers.vx] -
                  (int16_t)state.registers.V[instruction.registers.vy];
    uint8_t carry_flag = sub >= 0;
    state.registers.V[instruction.registers.vx] = (uint8_t)(sub & 0xFF);
    state.registers.V[0xF] = carry_flag;
  } break;

  case INST_4XNN:
    state.registers.PC += 2 * (state.registers.V[instruction.registers.vx] !=
                               instruction.address.NN);
    break;

  case INST_8XY0:
    state.registers.V[instruction.registers.vx] =
        state.registers.V[instruction.registers.vy];
    break;

  case INST_8XY6: {
    // NOTE: Some implementations seem to shift VY too??
    uint8_t carry_flag = state.registers.V[instruction.registers.vx] & 0x01;
    state.registers.V[instruction.registers.vx] >>= 1;
    state.registers.V[0xF] = carry_flag;
  } break;

  case INST_5XY0:
    state.registers.PC += 2 * (state.registers.V[instruction.registers.vx] ==
                               state.registers.V[instruction.registers.vy]);
    break;

  case INST_8XY1:
    state.registers.V[instruction.registers.vx] |=
        state.registers.V[instruction.registers.vy];
    break;

  case INST_8XYE: {
    // NOTE: Some implementations seem to shift VY too??
    uint8_t carry_flag =
        (state.registers.V[instruction.registers.vx] & 0x80) >> 7;
    state.registers.V[instruction.registers.vx] <<= 1;
    state.registers.V[0xF] = carry_flag;
  } break;

  case INST_7XNN:
    state.registers.V[instruction.registers.vx] += instruction.address.NN;
    break;

  case INST_8XY2:
    state.registers.V[instruction.registers.vx] &=
        state.registers.V[instruction.registers.vy];
    break;

  case INST_FX55:
    // NOTE: Apparently some implementations increment I after the loop??
    for (size_t i = 0; i <= instruction.registers.vx; i++) {
      state.memory[state.registers.I + i] = state.registers.V[i];
    }
    break;

  case INST_9XY0:
    state.registers.PC += 2 * (state.registers.V[instruction.registers.vx] !=
                               state.registers.V[instruction.registers.vy]);
    break;

  case INST_8XY3:
    state.registers.V[instruction.registers.vx] ^=
        state.registers.V[instruction.registers.vy];
    break;

  case INST_FX33: {
    uint8_t val = state.registers.V[instruction.registers.vx];
    state.memory[state.registers.I] = val / 100;
    state.memory[state.registers.I + 1] = (val / 10) % 10;
    state.memory[state.registers.I + 2] = val % 10;
  } break;

  case INST_ANNN:
    state.registers.I = instruction.address.NNN;
    break;

  case INST_8XY4: {
    uint16_t sum = (uint16_t)state.registers.V[instruction.registers.vx] +
                   (uint16_t)state.registers.V[instruction.registers.vy];
    uint8_t carry_flag = sum > 255;
    state.registers.V[instruction.registers.vx] = (uint8_t)(sum & 0xFF);
    state.registers.V[0xF] = carry_flag;
  } break;

  case INST_1NNN:
    state.registers.PC = instruction.address.NNN;
    break;

  case INST_EX9E:
    state.registers.PC += 2 * (state.keyboard[instruction.registers.vx] == 1);
    break;

  case INST_EXA1:
    state.registers.PC += 2 * (state.keyboard[instruction.registers.vx] == 0);
    break;

  case INST_FX07:
    state.registers.V[instruction.registers.vx] = state.registers.T.delay;
    break;

  case INST_FX0A:
    Platform::block_for_input();
    // TODO: Blocking wait for input (use the platform)
    break;

  case INST_FX15:
    state.registers.T.delay = state.registers.V[instruction.registers.vx];
    break;

  case INST_FX18:
    state.registers.T.sound = state.registers.V[instruction.registers.vx];
    break;

  case INST_FX1E:
    state.registers.I += state.registers.V[instruction.registers.vx];
    break;

  case INST_FX29:
    state.registers.I = state.registers.V[instruction.registers.vx] * 5;
    break;

  case INST_FX65:
    for (size_t i = 0; i <= instruction.registers.vx; i++) {
      state.registers.V[i] = state.memory[state.registers.I + i];
    }
    break;

  case INST_UNKNOWN:
    Platform::except_unknown_inst();
    break;
  }
}

void tick(State &state) {
  Instruction inst = decode_next_intruction(state);
  execute_decoded_instruction(state, inst);
}

void decrement_timers(State &state) {
  state.registers.T.delay =
      state.registers.T.delay > 0 ? state.registers.T.delay - 1 : 0;

  state.registers.T.sound =
      state.registers.T.sound > 0 ? state.registers.T.sound - 1 : 0;
}

} // namespace Pot8to
