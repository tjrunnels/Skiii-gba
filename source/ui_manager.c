#include "skiii.h"

void change_ui_state(ProgramState state) {
  if (state == MENU) {
    // load main menu objects into VRAM

    // load main menu objects into OAM

    // unload everything else
  } else if (state == OPTIONS) {
    // load options screen objects into vram

    // load options screen objects into OAM

    // unload everything else
  }
}
