#include "boot_loader.h"
#include "skiii.h"
#include "snow_floor.h"
#include "ui_manager.h"
#include <tonc.h>

// GBA resolution: 240 x 160


int main() {
  // Setup the GBA program
  ProgramState game_state = MENU;
  change_ui_state(game_state);

  // Init interrupts and VBlank irq.
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  // Load all assets that have a fixed memory home for the whole program.
  load_boot_assets();
  enable_running_snow_background_0();

  // Scroll around some
  short int SCROLL_DELTA_X = 192/8;
  short int SCROLL_DELTA_Y = 64;

  // main game loop
  while (1) {
    VBlankIntrWait(); //  Wait until the last frame finishes drawing
    key_poll();       //  snapshot of keys i think?

    SCROLL_DELTA_Y += 1;
    SCROLL_DELTA_X += key_tri_shoulder();

    REG_BG0HOFS = SCROLL_DELTA_X;
    REG_BG0VOFS = SCROLL_DELTA_Y;
  }

  return 0;
}
