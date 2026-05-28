#include "boot_loader.h"
#include "skiii_logo.h"
#include "snow_floor.h"
#include <tonc.h>

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
 * 0 / colors 0-15     unassigned                            TODO
 * 1-15                unassigned                            TODO
 *
 * Background charblocks: tile_mem[CBB], VRAM starting at 0x06000000
 * ---------------------------------------------------------------------------
 * CBB 0-1             snow_floor tiles                      Uses SBB 0-15 space
 * CBB 2               skiii_logo BG tiles                   Uses SBB 16-23 space               
 * CBB 3               screenblock/map storage               Uses SBB 24-31 space
 *
 * Background screenblocks: se_mem[SBB], 2 KB each
 * ---------------------------------------------------------------------------
 * SBB 28              skiii_logo BG map                     TODO
 * SBB 30              snow_floor map                        64x32 BG0 map
 *
 * Object tile memory: object VRAM, commonly tile_mem[4], 0x06010000
 * ---------------------------------------------------------------------------
 * Tile range          Owner / asset                         Notes
 * TODO                unassigned                            TODO
 *
 * OAM: oam_mem, 0x07000000
 * ---------------------------------------------------------------------------
 * OAM index           Owner / asset                         Notes
 * TODO                unassigned                            TODO
 */

void load_boot_assets(void) {

  // SNOW_FLOOR
  // Load snow_floor palette into background palette memory.
  memcpy16(pal_bg_mem, snow_floorPal, snow_floorPalLen / sizeof(u16));

  // Load snow_floor tiles into background charblock 0.
  memcpy32(&tile_mem[0][0], snow_floorTiles, snow_floorTilesLen / sizeof(u32));

  // Load snow_floor map into screenblock 30.
  memcpy32(&se_mem[30][0], snow_floorMap, snow_floorMapLen / sizeof(u32));


  // SKIII_LOGO
  // TODO: Load skiii_logo palette into its planned background palette bank.
  memcpy16(pal_bg_mem + 32, skiii_logoPal, skiii_logoPalLen / sizeof(u16));

  // TODO: Load skiii_logo tiles into background charblock 1.
  memcpy32(&tile_mem[2][0], skiii_logoTiles, skiii_logoTilesLen / sizeof(u32));

  // TODO: Load skiii_logo map into screenblock 28.
  memcpy32(&se_mem[28][0], skiii_logoMap, skiii_logoMapLen / sizeof(u32));

}

void enable_running_snow_background_0(void) {
  // set up BG0 for a 4bpp 64x32t map, using
  // using charblock 0 and screenblock 31

  REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x64;
  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;  

  REG_BG1CNT = BG_CBB(2) | BG_SBB(28) | BG_4BPP | BG_REG_32x32;
  REG_DISPCNT = DCNT_BG1 | REG_DISPCNT;  

}