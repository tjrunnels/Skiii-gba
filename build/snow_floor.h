
//{{BLOCK(snow_floor)

//======================================================================
//
//	snow_floor, 256x512@4, 
//	+ palette 16 entries, not compressed
//	+ 616 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x64 
//	Total size: 32 + 19712 + 4096 = 23840
//
//	Time-stamp: 2026-05-28, 16:42:16
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SNOW_FLOOR_H
#define GRIT_SNOW_FLOOR_H

#define snow_floorTilesLen 19712
extern const unsigned int snow_floorTiles[4928];

#define snow_floorMapLen 4096
extern const unsigned short snow_floorMap[2048];

#define snow_floorPalLen 32
extern const unsigned short snow_floorPal[16];

#endif // GRIT_SNOW_FLOOR_H

//}}BLOCK(snow_floor)
