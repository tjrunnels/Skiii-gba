#ifndef SKIII_H
#define SKIII_H
#include "mgba_log.h"

typedef enum { MENU, OPTIONS, CREDITS, SKI, RESULTS } ProgramState;
typedef enum { STANDBY, BEGIN, PLAYING, DONE } SkiStartAnimation;
typedef enum { _STANDBY, _BEGIN, _PLAYING, _DONE } ResultScreenAnimation;

extern ProgramState game_state;
void set_game_state(ProgramState new_state);
static void print(char message[20]) {
  mgbaprintf(message);
}
#endif
