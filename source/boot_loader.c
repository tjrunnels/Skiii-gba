#include "boot_loader.h"
#include "skiii_logo.h"
#include "snow_floor.h"
#include "snow_floor32.h"
#include "start.h"
#include <tonc.h>
#include "square_objects.h"

/*
 * Boot Asset Memory Map
 * =====================
 *
 * This file is the place to document the assets that are loaded once at boot.
 * Keep this comment updated whenever an asset gets a permanent place in VRAM,
 * palette memory, screenblock memory, or OAM.
 *
 * Background palette memory: pal_bg_mem, 0x05000000
 * ---------------------------------------------------------------------------
 * Bank / range        Owner / asset                         Notes
 * 0 / colors 0-15     snow_floor on BG0                     TODO
 * 1 / colors 16-31    skiii_logo on BG1                     TODO
 * 2-15                unassigned                            TODO
 *
 * Object palette memory: pal_obj_mem, 0x05000200
 * ---------------------------------------------------------------------------
 * Bank / range        Owner / asset                         Notes
 * 0 / colors 0-15     start                                 pal_obj_mem
 * 1 / colors 16-31    square_objects                        pal_obj_mem + 16
 * 2-15                unassigned
 *
 * Background charblocks: tile_mem[CBB], VRAM starting at 0x06000000
 * ---------------------------------------------------------------------------
 * CBB 0-1             snow_floor tiles                      Uses SBB 0-15 space
 * CBB 2               skiii_logo BG tiles                   Uses SBB 16-23 space               
 * CBB 3               screenblock/map storage               Uses SBB 24-31 space
 *
 * Background screenblocks: se_mem[SBB], 2 KB each, 32x32 each
 * ---------------------------------------------------------------------------
 * SBB 0               skiii_logo BG map                     TODO
 * SBB 30,31           snow_floor map                        64x32 BG0 map
 *
 * Object tile memory: object VRAM, commonly tile_mem[4], 0x06010000
 * ---------------------------------------------------------------------------
 * Each index is one 8x8 tile (32 bytes). The PPU does not care how tall the
 * PNG was; it reads however many tiles ATTR1_SIZE says.
 *
 * START image is 64x24 (24 tiles). START's object is 64x32, which reads 32
 * tiles. Those two numbers are the collision:
 *
 *          8 tiles wide (64px)
 *        +--+--+--+--+--+--+--+--+
 *   0-7  |        START row 0        |  real pixels
 *  8-15  |        START row 1        |  real pixels
 * 16-23  |        START row 2        |  real pixels (image ends)
 * 24-31  |        MUST BE 0'd        |  64x32 object still reads this
 *        +--+--+--+--+--+--+--+--+
 * 32-47  | square_objects (16 tiles) |  safe home after the 32-tile slot
 *        +--+--+--+--+--+--+--+--+
 *
 * Tile range          Owner / asset                         Notes
 * 0-23                start graphics                        grit: 64x24@4, 24 tiles
 * 24-31               also start graphics but unneeded      ATTR1_SIZE_64x32 will read ids 0-31. Make sure this is 0'd out or you could get artifacts.
 * 32-47               square_objects (intended)             ATTR1_SIZE_16x16
 * *
 * OAM: oam_mem, 0x07000000
 * ---------------------------------------------------------------------------
 * OAM index           Owner / asset                         Notes
 * 0                   start icon                            allObjects[0], ATTR2_ID(0 since it's not a sheet, just a single image)
 * 1                   menu flag                             allObjects[1], ATTR2_ID(sheet base)
 * 2-127               hidden by oam_init
 */

OBJ_ATTR allObjects[128];

BootReturn load_boot_assets(void) {

  BootReturn to_return;

  // SNOW_FLOOR
  // Load snow_floor palette into background palette memory.
  memcpy16(pal_bg_mem, snow_floor32Pal, snow_floor32PalLen / sizeof(u16));

  // Load snow_floor tiles into background charblock 0.
  memcpy32(&tile_mem[0][0], snow_floor32Tiles, snow_floor32TilesLen / sizeof(u32));

  // Load snow_floor map into screenblock 30 and 31 (since it's 64x32)
  memcpy32(&se_mem[30][0], snow_floor32Map, snow_floor32MapLen / sizeof(u32));


  // SKIII_LOGO
  // Load skiii_logo palette into its planned background palette bank.
  memcpy16(pal_bg_mem + 16, skiii_logoPal, skiii_logoPalLen / sizeof(u16));

  // Load skiii_logo tiles into background charblock 2.
  memcpy32(&tile_mem[2][0], skiii_logoTiles, skiii_logoTilesLen / sizeof(u32));

  // Load skiii_logo map into screenblock 0.
  memcpy32(&se_mem[24][0], skiii_logoMap, skiii_logoMapLen / sizeof(u32));


  ///START image
  // Load start palette into object palette memory
  memcpy16(pal_obj_mem, startPal, startPalLen / sizeof(u16));

  // Load start tiles into object tile memory (only loads the 24 tiles worth of data that the image is made of)
  memcpy32(&tile_mem[4][0], startTiles, startTilesLen / sizeof(u32));
  // Zero out the rest of the 32-tile slot that the ATTR1_SIZE_64x32 will read, to avoid artifacts.
  memset32(&tile_mem[4][startTilesLen / sizeof(TILE)], 0, 8 * sizeof(TILE) / sizeof(u32)); // 8 tiles left to fill out the 32-tile slot
/*                          ^^^^^^^^ tiles = 24                    ^^^^^^^^ words = 64        */
  // Init all of OAm objects
  oam_init(allObjects, 128);

  //use position 0
  OBJ_ATTR *startIcon = &allObjects[0];
  startIcon->attr0 = ATTR0_Y(100) | ATTR0_REG | ATTR0_4BPP | ATTR0_SHAPE(1);
  startIcon->attr1 = ATTR1_X(88) | ATTR1_SIZE_64x32; // the object now owns 32 tiles worth of RAM, not just the 24
  startIcon->attr2 = ATTR2_ID(0);


  // Load the flag icon into OAM
  // First object palette memory
  memcpy16(pal_obj_mem + 16, square_objectsPal, square_objectsPalLen / sizeof(u16));

  int START_BUTTON_TILE_COUNT = 32; // because the OBJECT that holds the tiles is 32 "tiles" long

  //then load all the tiles into object tile memory
  memcpy32(&tile_mem[4][START_BUTTON_TILE_COUNT], square_objectsTiles, square_objectsTilesLen / sizeof(u32));

  //use position 1 of OAM
  OBJ_ATTR *flag_icon = &allObjects[1];
  flag_icon->attr0 = ATTR0_REG | ATTR0_4BPP;
  flag_icon->attr1 = ATTR1_SIZE_16x16;
  flag_icon->attr2 = ATTR2_ID(START_BUTTON_TILE_COUNT) | ATTR2_PALBANK(1);

  to_return.flag_icon = flag_icon;

  oam_copy(oam_mem, allObjects, 2);

  return to_return;
}

void enable_running_snow_background_0(void) {

  /*
   * REG_BG0CNT is the control register for background layer 0.
   *
   * The GBA stores several different BG settings inside one 16-bit register.
   * Each setting owns a few specific bits. The bitwise OR operator (`|`)
   * combines those settings into one final register value.
   *
   * Think of the line below as filling out one compact hardware form:
   *
   *   BG_PRIO(0)    -> This background should be rendered rearmost.
   *                    priority 0 = frontmost
   *                    priority 1
   *                    priority 2
   *                    priority 3 = rearmost
   * 
   *   BG_CBB(0)     -> Use background charblock 0 for tile graphics.
   *                    This must match where load_boot_assets copied
   *                    snow_floorTiles.
   *
   *   BG_SBB(30)    -> Use screenblock 30 for the tile map.
   *                    This must match where load_boot_assets copied
   *                    snow_floorMap. A 32x64 map uses two screenblocks, so
   *                    snow_floor occupies SBB 30 and SBB 31.
   *
   *   BG_4BPP       -> Read the tile graphics as 4 bits per pixel.
   *                    This must match the GRIT flag -gB4.
   *
   *   BG_REG_32x64  -> Read the map as 32 tiles wide and 64 tiles tall.
   *                    This must match the map size GRIT generated from the
   *                    source image.
   *
   * To set up another regular background, use the same recipe:
   *
   *   1. Copy its tiles into a free charblock.
   *   2. Copy its map into a free screenblock range.
   *   3. Copy its palette colors into the planned palette bank/range.
   *   4. Point REG_BGxCNT at those same charblock/screenblock numbers.
   *   5. Use bpp and map-size flags that match the exported asset.
   */
  REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;

  /*
   * REG_DISPCNT controls the whole display.
   *
   * DCNT_MODE0 selects tiled background mode 0. Mode 0 gives you up to four
   * regular tiled backgrounds: BG0, BG1, BG2, and BG3.
   *
   * DCNT_BG0 makes BG0 visible. Loading data into VRAM is not enough by itself;
   * the display control register also has to enable the layer.
   */
  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;  

  REG_BG1CNT = BG_PRIO(0) | BG_CBB(2) | BG_SBB(24) | BG_4BPP | BG_REG_32x32;

  /*
   * Flip the BG1 on with the existing display settings.
   */
  REG_DISPCNT = DCNT_BG1 | REG_DISPCNT;  

  // Center Skiii logo on screen
  REG_BG1HOFS = -55;
  REG_BG1VOFS = -25;

  // Turn on objects
  REG_DISPCNT = REG_DISPCNT | DCNT_OBJ | DCNT_OBJ_1D;
}
