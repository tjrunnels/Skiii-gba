#include "skiii.h"
#include <tonc.h>
#include "menu_navigation_manager.h"
#include "mgba_log.h"

static void printHi(void) {
  mgbaprintf("Hi\n");
}

void change_ui_state(ProgramState state) {
  if (state == MENU) {
    // TODO: Turn on the menu background layers that were loaded at boot.
    // unload everything else

    // startup the menu navigation system
    static MenuNode main_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 120, 40, NULL, &main_menu_nodes[1], NULL, NULL, printHi },
      { 120, 80, &main_menu_nodes[0], &main_menu_nodes[2], NULL, NULL, NULL },
      { 120, 120, &main_menu_nodes[1], NULL, NULL, NULL, NULL }
    };
    menu_set_active(&main_menu_nodes[0]);

  } else if (state == OPTIONS) {
    // load options screen objects into vram

    // load options screen objects into OAM

    // unload everything else
  }
}
