#include "skiii.h"
#include "snow_floor.h"
#include "ui_manager.h"
#include <string.h>
#include <tonc.h>

int main() {
  // Setup the GBA program
  ProgramState game_state = MENU;
  change_ui_state(game_state);

  // Init interrupts and VBlank irq.
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  // -- Snow Floor is always used so i'll leave it in main
  // Load snow_floor palette
  memcpy16(pal_bg_mem, snow_floorPal, snow_floorPalLen / sizeof(u16));
  // Load snow_floor tiles into CBB 0
  memcpy32(&tile_mem[0][0], snow_floorTiles, snow_floorTilesLen / sizeof(u32));
  // Load snow_floor into SBB 30
  memcpy32(&se_mem[30][0], snow_floorMap, snow_floorMapLen / sizeof(u32));

  // DEBUG
  //  set up BG0 for a 4bpp 64x32t map, using
  //    using charblock 0 and screenblock 31
  REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_64x32;
  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

  int DEBUG_SCROLL_STATE = 0;

  // Scroll around some
  short int SCROLL_DELTA_X = 192;
  short int SCROLL_DELTA_Y = 64;

  // -- END OF DEBD

  // main game loop
  while (1) {
    VBlankIntrWait(); // Wait until the last frame finishes drawing
    key_poll();       // snapshot of keys i think?

    if (DEBUG_SCROLL_STATE) {
      SCROLL_DELTA_X += 1;
    }

    if (key_hit(KEY_LEFT)) {
      DEBUG_SCROLL_STATE = !DEBUG_SCROLL_STATE;
    }

    SCROLL_DELTA_Y += key_tri_vert();

    REG_BG0HOFS = SCROLL_DELTA_X;
    REG_BG0VOFS = SCROLL_DELTA_Y;
  }

  return 0;
}
