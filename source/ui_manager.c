#include "skiii.h"
#include <tonc.h>
#include "menu_navigation_manager.h"
#include "mgba_log.h"
#include "boot_loader.h"
#include "ui_manager.h"

static void printHi(void) {
  mgbaprintf("Hi\n");
}
static void change_state_to_menu(void) {
  change_ui_state(MENU);
}
static void change_state_to_options(void) {
  change_ui_state(OPTIONS);
}

static void hide_all_objects(void){
  for (int i; i < 128; i++) {
    (&allObjects[i])->attr0 |= 1 << 9; // sets the hide bit on all
  }
  oam_copy(oam_mem, allObjects, 128);
}

void change_ui_state(ProgramState state) {
  if (state == MENU) {
    // TODO: Turn on the menu background layers that were loaded at boot.
    // unload everything else
    hide_all_objects();

    // unhide "Start" menu option
    (&allObjects[0])->attr0 &= ~(1 << 9); //the 9th bit of OAM is the difference between hidden (10) and regular (00)
    (&allObjects[1])->attr0  &= ~(1 << 9);
    (&allObjects[2])->attr0  &= ~(1 << 9);
    oam_copy(oam_mem, allObjects, 3);


    // startup the menu navigation system
    static MenuNode main_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 68, 100, NULL, &main_menu_nodes[1], NULL, NULL, printHi },
      { 68, 130, &main_menu_nodes[0], &main_menu_nodes[2], NULL, NULL, change_state_to_options },
      { 0, 0 , &main_menu_nodes[1], NULL, NULL, NULL, NULL }
    };
    menu_set_active(&main_menu_nodes[0]);

  } else if (state == OPTIONS) {
    hide_all_objects();

    // startup the menu navigation system
    static MenuNode main_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 68, 100, NULL, &main_menu_nodes[1], NULL, NULL, change_state_to_menu },
    };
    menu_set_active(&main_menu_nodes[0]);


    // load options screen objects into vram

    // load options screen objects into OAM

    // unload everything else
  }
}
