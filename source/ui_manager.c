#include "skiii.h"
#include <tonc.h>


void change_ui_state(ProgramState state) {
  if (state == MENU) {
    // TODO: Turn on the menu background layers that were loaded at boot.
    // unload everything else
  } else if (state == OPTIONS) {
    // load options screen objects into vram

    // load options screen objects into OAM

    // unload everything else
  }
}
