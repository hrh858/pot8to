#ifdef __APPLE__
#include "platform.h"
#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <sys/random.h>

namespace Platform {

Program pick_and_load_program() {
  Program loadedProgram = {}; // Initialize loaded program structure

  @autoreleasepool {
    [NSApp activateIgnoringOtherApps:YES];

    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:NO];

    // Use allowedContentTypes instead of allowedFileTypes
    if (@available(macOS 14.0, *)) { // UTType introduced in macOS 14
      [panel setAllowedContentTypes:@[
        [UTType typeWithFilenameExtension:@"ch8"],
        [UTType typeWithFilenameExtension:@"rom"],
        [UTType typeWithFilenameExtension:@"bin"]
      ]];
    } else {
      // Fallback for older macOS versions if needed
      [panel setAllowedFileTypes:@[ @"ch8", @"rom", @"bin" ]];
    }

    [panel setTitle:@"Select a Chip8 ROM file"];
    [panel setPrompt:@"Load"];

    NSInteger result = [panel runModal];
    if (result == NSModalResponseOK) {
      NSURL *selectedFile = [[panel URLs] firstObject];
      NSData *fileData = [NSData dataWithContentsOfURL:selectedFile];
      if (fileData && fileData.length < POT8TO_PROGRAM_MEMORY) {
        loadedProgram.size = fileData.length;
        [fileData getBytes:loadedProgram.buffer length:fileData.length];
      } else {
        // Handle error: file too large or failed to read
        NSLog(@"Error: File is too large or could not be read.");
      }
    } else {
      // Handle user cancellation if necessary
      NSLog(@"User canceled the file selection.");
    }
  }

  return loadedProgram;
}

void render_and_play_sound(
    const uint8_t screen[POT8TO_DISPLAY_HEIGHT][POT8TO_DISPLAY_WIDTH],
    bool sound) {}
} // namespace Platform

uint8_t Platform::rnd_8bits() {
  @autoreleasepool {
    uint8_t rnd;
    if (getentropy(&rnd, 1) == 0) {
      return rnd;
    } else {
      return 42;
    }
  }
}

void Platform::except_unknown_inst() {
  NSAlert *alert = [[NSAlert alloc] init];
  [alert setMessageText:@"Pot8to will quit"];
  [alert setInformativeText:@"The emulator found an unknown instruction"];
  [alert addButtonWithTitle:@"Quit"];
  [alert runModal];
  [NSApp terminate:nil];
};

void Platform::block_for_input() {}

#endif
