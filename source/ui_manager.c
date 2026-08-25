#include "skiii.h"
#include <tonc.h>
#include "menu_navigation_manager.h"
#include "boot_loader.h"
#include "ui_manager.h"

// static void printHi(void) {
//   mgbaprintf("Hi\n");
// }
static void change_state_to_menu(void) {
  set_game_state(MENU);
  change_ui_state(game_state);
}
static void change_state_to_options(void) {
  set_game_state(OPTIONS);
  change_ui_state(game_state);
}
static void change_state_to_ski(void) {
  print("chang state 2 ski");
  set_game_state(SKI);
  change_ui_state(game_state);
}

static void hide_all_objects(void){
  for (int i = 0; i < 128; i++) {
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


    // startup the menu navigation system
    static MenuNode main_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 68, 100, NULL, &main_menu_nodes[1], NULL, NULL, change_state_to_ski },
      { 68, 130, &main_menu_nodes[0], NULL, NULL, NULL, change_state_to_options },
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
  } else if (state == SKI) {
    hide_all_objects();
    
    //unhide the player icon
    (&allObjects[3])->attr0  &= ~(1 << 9);

    // TODO: delete.  Debug: select to exit SKI mode
    static MenuNode main_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { -50, -50, NULL, NULL, NULL, NULL, change_state_to_menu },
    };
    menu_set_active(&main_menu_nodes[0]);
    oam_copy(oam_mem, allObjects, 4);

  }
}
