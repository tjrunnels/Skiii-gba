
//{{BLOCK(results_backdrop)

//======================================================================
//
//	results_backdrop, 256x512@4, 
//	+ palette 16 entries, not compressed
//	+ 38 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x64 
//	Total size: 32 + 1216 + 4096 = 5344
//
//	Time-stamp: 2026-09-08, 16:43:33
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_RESULTS_BACKDROP_H
#define GRIT_RESULTS_BACKDROP_H

#define results_backdropTilesLen 1216
extern const unsigned int results_backdropTiles[304];

#define results_backdropMapLen 4096
extern const unsigned short results_backdropMap[2048];

#define results_backdropPalLen 32
extern const unsigned short results_backdropPal[16];

#endif // GRIT_RESULTS_BACKDROP_H

//}}BLOCK(results_backdrop)
