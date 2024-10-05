#import "platform.h"
#include "pot8to.cpp"
#import <Cocoa/Cocoa.h>

@interface Chip8View : NSView
@property(nonatomic, assign) Pot8to::State *emulatorState;
@end

@implementation Chip8View
- (void)drawRect:(NSRect)dirtyRect {
  [super drawRect:dirtyRect];

  CGFloat pixelSize = 15.0;

  [[NSColor colorWithCalibratedRed:0.10 green:0.11 blue:0.15
                             alpha:1.0] setFill];
  NSRectFill(dirtyRect);

  [[NSColor colorWithCalibratedRed:0.56 green:0.80 blue:0.98
                             alpha:1.0] setFill];

  for (size_t y = 0; y < POT8TO_DISPLAY_HEIGHT; y++) {
    for (size_t x = 0; x < POT8TO_DISPLAY_WIDTH; x++) {
      // Invert the Y-axis when accessing the display buffer
      uint8_t pixel =
          self.emulatorState->display[POT8TO_DISPLAY_HEIGHT - 1 - y][x];
      if (pixel) {
        NSRect rect =
            NSMakeRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
        NSRectFill(rect);
      }
    }
  }
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(strong) NSWindow *window;
@property(strong) Chip8View *chip8View;
@property(strong) NSTimer *emulationTimer;
@end

@implementation AppDelegate {
  Pot8to::State *emulatorState;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  Platform::Program rom = Platform::pick_and_load_program();
  emulatorState = new Pot8to::State(Pot8to::initialize(rom));

  CGFloat pixelSize = 15.0;
  NSRect windowRect = NSMakeRect(0, 0, POT8TO_DISPLAY_WIDTH * pixelSize,
                                 POT8TO_DISPLAY_HEIGHT * pixelSize);

  self.window = [[NSWindow alloc]
      initWithContentRect:windowRect
                styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable)
                  backing:NSBackingStoreBuffered
                    defer:NO];
  [self.window setTitle:@"Chip-8 Emulator"];

  self.chip8View = [[Chip8View alloc] initWithFrame:windowRect];
  self.chip8View.emulatorState = emulatorState;
  [self.window setContentView:self.chip8View];

  [self.window makeKeyAndOrderFront:nil];

  self.emulationTimer =
      [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60.0)
                                       target:self
                                     selector:@selector(tickEmulator)
                                     userInfo:nil
                                      repeats:YES];
}

- (void)tickEmulator {
  Pot8to::tick(*emulatorState);
  [self.chip8View setNeedsDisplay:YES];
}

- (void)dealloc {
  [self.emulationTimer invalidate];
  delete emulatorState;
}
@end

int main(int argc, const char *argv[]) {
  @autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];

    AppDelegate *delegate = [[AppDelegate alloc] init];
    [NSApp setDelegate:delegate];

    [NSApp run];
  }

  return 0;
}
