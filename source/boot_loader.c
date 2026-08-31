#include "boot_loader.h"
#include "skiii_logo.h"
#include "snow_floor.h"
#include "snow_floor32.h"
#include "start.h"
#include "options.h"
#include <tonc.h>
#include "square_objects.h"

/*
 * Boot Asset Memory Map
 * =====================
 *
 * Background Palette (pal_bg_mem, 0x05000000):
 *   Bank 0 (colors 0-15)   : snow_floor
 *   Bank 1 (colors 16-31)  : skiii_logo
 *
 * Object Palette (pal_obj_mem, 0x05000200):
 *   Bank 0 (colors 0-15)   : start icon
 *   Bank 1 (colors 16-31)  : options icon
 *   Bank 2 (colors 32-47)  : flags & player
 *
 * Background Charblocks (tile_mem[CBB], 16 KB each):
 *   CBB 0-1                : snow_floor tiles (spans 521 tiles across CBB 0 and CBB 1)
 *   CBB 2                  : skiii_logo tiles (0-79) & TTE font tiles (100+)
 *   CBB 3                  : Screenblock (map) storage area
 *
 * Background Screenblocks (se_mem[SBB], 2 KB each):
 *   SBB 24                 : skiii_logo map (32x32)
 *   SBB 26                 : TTE text map (32x32)
 *   SBB 30-31              : snow_floor map (64x32)
 *
 * Object VRAM (tile_mem[4], 0x06010000):
 *   Tiles 0-31             : start icon (64x32 slot)
 *   Tiles 32-63            : options icon (64x32 slot)
 *   Tiles 64-71            : square_objects (flags)
 *   Tiles 72+              : player animation frames
 */

OBJ_ATTR allObjects[128];

BootReturn load_boot_assets(void) {

  BootReturn to_return;

  // Init all of OAm objects
  oam_init(allObjects, 128);

  // SNOW_FLOOR
  /////////////////////////// 
  // Load snow_floor palette into background palette memory.
  memcpy16(pal_bg_mem, snow_floor32Pal, snow_floor32PalLen / sizeof(u16));

  // Load snow_floor tiles into background charblock 0.
  memcpy32(&tile_mem[0][0], snow_floor32Tiles, snow_floor32TilesLen / sizeof(u32));

  // Load snow_floor map into screenblock 30 and 31 (since it's 64x32)
  memcpy32(&se_mem[30][0], snow_floor32Map, snow_floor32MapLen / sizeof(u32));


  // SKIII_LOGO
  /////////////////////////// 
  // Load skiii_logo palette into its planned background palette bank.
  memcpy16(pal_bg_mem + 16, skiii_logoPal, skiii_logoPalLen / sizeof(u16));

  // Load skiii_logo tiles into background charblock 2.
  memcpy32(&tile_mem[2][0], skiii_logoTiles, skiii_logoTilesLen / sizeof(u32));

  // Load skiii_logo map into screenblock 0.
  memcpy32(&se_mem[24][0], skiii_logoMap, skiii_logoMapLen / sizeof(u32));


  int LOADED_PAL_COUNT = 0;
  int LOADED_TILE_COUNT = 0;

  ///START image
  /////////////////////////// 
  // Load start palette into object palette memory
  memcpy16(pal_obj_mem + LOADED_PAL_COUNT, startPal, startPalLen / sizeof(u16));

  // Load start tiles into object tile memory (only loads the 24 tiles worth of data that the image is made of)
  memcpy32(&tile_mem[4][0], startTiles, startTilesLen / sizeof(u32));
  // Zero out the rest of the 32-tile slot that the ATTR1_SIZE_64x32 will read, to avoid artifacts.
  memset32(&tile_mem[4][startTilesLen / sizeof(TILE)], 0, 8 * sizeof(TILE) / sizeof(u32)); // 8 tiles left to fill out the 32-tile slot
/*                          ^^^^^^^^ tiles = 24                    ^^^^^^^^ words = 64        */

  //use position 0
  OBJ_ATTR *start_icon = &allObjects[0];
  start_icon->attr0 = ATTR0_Y(100) | ATTR0_HIDE | ATTR0_4BPP | ATTR0_SHAPE(1);
  start_icon->attr1 = ATTR1_X(88) | ATTR1_SIZE_64x32; // the object now owns 32 tiles worth of RAM, not just the 24
  start_icon->attr2 = ATTR2_ID(0);
  to_return.start_icon = start_icon;

  LOADED_PAL_COUNT += 16;
  LOADED_TILE_COUNT += 32;

  ///OPTIONS image
  /////////////////////////// 
  // Load options palette into object palette memory
  memcpy16(pal_obj_mem + LOADED_PAL_COUNT, optionsPal, optionsPalLen / sizeof(u16));

  // Load options tiles into object tile memory (only loads the 16 tiles worth of data that the image is made of)
  memcpy32(&tile_mem[4][LOADED_TILE_COUNT], optionsTiles, optionsTilesLen / sizeof(u32));
  // Zero out the rest of the 32-tile slot that the ATTR1_SIZE_64x32 will read, to avoid artifacts.
  memset32(&tile_mem[4][optionsTilesLen / sizeof(TILE)], 0, 8 * sizeof(TILE) / sizeof(u32)); // 16 tiles left to fill out the 32-tile slot
  /*                          ^^^^^^^^ tiles = 16                    ^^^^^^^^ words = 64        */

  //use position 1 of OAM
  OBJ_ATTR *options_icon =  &allObjects[1];
  options_icon->attr0 = ATTR0_Y(130) | ATTR0_HIDE | ATTR0_4BPP | ATTR0_SHAPE(1);
  options_icon->attr1 = ATTR1_X(88) | ATTR1_SIZE_64x32; // the object now owns 32 tiles worth of RAM, not just the 24
  options_icon->attr2 = ATTR2_ID(LOADED_TILE_COUNT)  | ATTR2_PALBANK(1);
  to_return.options_icon = options_icon;


  LOADED_PAL_COUNT += 16;
  LOADED_TILE_COUNT += 32;

  // FLAG icon
  ///////////////////////////   
  // Load the flag icon into OAM
  // First object palette memory
  memcpy16(pal_obj_mem + LOADED_PAL_COUNT, square_objectsPal, square_objectsPalLen / sizeof(u16));

  //then load all the tiles into object tile memory
  memcpy32(&tile_mem[4][LOADED_TILE_COUNT], square_objectsTiles, square_objectsTilesLen / sizeof(u32));

  //use position 2 of OAM
  OBJ_ATTR *flag_icon = &allObjects[2];
  flag_icon->attr0 = ATTR0_4BPP | ATTR0_HIDE;
  flag_icon->attr1 = ATTR1_SIZE_16x16;
  flag_icon->attr2 = ATTR2_ID(LOADED_TILE_COUNT) | ATTR2_PALBANK(2);

  // left/right of 5 flag groups
  for(int i = 10; i < 20; i++) {
    OBJ_ATTR *flag_icon_i = &allObjects[i];
    flag_icon_i->attr0 = ATTR0_4BPP | ATTR0_HIDE | ATTR0_Y(255);
    flag_icon_i->attr1 = ATTR1_SIZE_16x16 | ATTR1_X(255);
    flag_icon_i->attr2 = ATTR2_ID(LOADED_TILE_COUNT) | ATTR2_PALBANK(2) | ATTR2_PRIO(3);
  }

  to_return.flag_icon = flag_icon;

  // PLAYER icon
  ///////////////////////////   
  // the palatte and sprites are already loaded
  // the player image will need to cycle between 3 different images
  const int PLAYER_IMAGE_BASE_INDEX = LOADED_TILE_COUNT + 8;

  //use position 3 of OAM
  OBJ_ATTR *player_icon =  &allObjects[3];
  player_icon->attr0 = ATTR0_4BPP | ATTR0_HIDE | ATTR0_Y(235);
  player_icon->attr1 = ATTR1_SIZE_16x16;
  player_icon->attr2 = ATTR2_ID(PLAYER_IMAGE_BASE_INDEX) | ATTR2_PALBANK(2) | ATTR2_PRIO(0);

  to_return.player_icon = player_icon;
  to_return.playerImageBaseIndex = PLAYER_IMAGE_BASE_INDEX;

  oam_copy(oam_mem, allObjects, 20);

  return to_return;
}

void enable_running_snow_background_0(void) {
  // BG0: Scrolling snow floor (rearmost)
  REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;

  // BG1: Skiii logo (toggled via ui_manager)
  REG_BG1CNT = BG_PRIO(1) | BG_CBB(2) | BG_SBB(24) | BG_4BPP | BG_REG_32x32;
  REG_BG1HOFS = 200;
  REG_BG1VOFS = 220;

  // BG2: Tonc Text Engine (TTE) sharing CBB 2 (font starting at tile index 100) and SBB 26 for map
  tte_init_se(
      2,                      // BG layer 2
      BG_CBB(2) | BG_SBB(26), // CBB 2, SBB 26
      SE_ID(100),             // Base screen entry: tile ID 100, palette bank 0
      0xF000,                 // Ink color 15, transparent background
      100,                    // Tile offset in CBB 2: load font starting at tile 100
      NULL,                   // Default sys8 font
      NULL                    // Default renderer
  );
  REG_BG2CNT |= BG_PRIO(0);
  tte_init_con(); // Connects stdio/iprintf/tte_printf to TTE

  // Enable Mode 0 with BG0, BG2 (text), and 1D mapped objects
  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}
