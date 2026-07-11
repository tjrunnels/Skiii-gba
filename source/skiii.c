#include "boot_loader.h"
#include "skiii.h"
#include "snow_floor.h"
#include "ui_manager.h"
#include "menu_navigation_manager.h"
#include "mgba_log.h"
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

  if (dlog_open()) {
    mgbaprintf("mGBA logging ready\n");
  }

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

    if(game_state != SKI) {
      // keybindings move the menu navigation system
      if (key_hit(KEY_UP)) {
        menu_nav_up();
      } else if (key_hit(KEY_DOWN)) {
        menu_nav_down();
      } else if (key_hit(KEY_LEFT)) {
        menu_nav_left();
      } else if (key_hit(KEY_RIGHT)) {
        menu_nav_right();
      } else if (key_hit(KEY_A)) {
        menu_select();
      }

      // mgbaprintf("Current menu node: (%d, %d)\n", get_current()->x, get_current()->y);
    }
  }

  return 0;
}
