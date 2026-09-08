#include "skiii.h"
#include <tonc.h>
#include "menu_navigation_manager.h"
#include "boot_loader.h"
#include "ui_manager.h"
#include "save_to_gba.h"

// static void printHi(void) {
//   mgbaprintf("Hi\n");
// }
static void change_state_to_menu(void) {
  tte_printf("#{es}");
  set_game_state(MENU);
  change_ui_state(game_state);
}
static void change_state_to_options(void) {
  tte_printf("#{es}");
  set_game_state(OPTIONS);
  change_ui_state(game_state);
}
static void change_state_to_ski(void) {
  print("chang state 2 ski");
  tte_printf("#{es}");
  set_game_state(SKI);
  change_ui_state(game_state);
}
static void turn_on_Skiii_logo(void) {
  REG_DISPCNT = DCNT_BG1 | REG_DISPCNT; //turn on the index 9 bit
}
static void turn_off_Skiii_logo(void) {
  // REG_DISPCNT = ~(DCNT_BG1 | ~REG_DISPCNT); //turn off the index 9 bit
  REG_DISPCNT &= 0b1111110111111111; //turn off the index 9 bit
}

static void turn_on_result_backdrop(void) {
  REG_DISPCNT = DCNT_BG3 | REG_DISPCNT; //turn on the index 9 bit
}
static void turn_off_result_backdrop(void) {
  REG_DISPCNT = ~(DCNT_BG3 | ~REG_DISPCNT); //turn off the index 9 bit
  REG_BG3VOFS = 0; // set location offscreen so it doesn't glitch later
}

static void hide_all_objects(void){
  // hide all sprites
  for (int i = 0; i < 60; i++) {
    (&allObjects[i])->attr0 |= 1 << 9; // sets the hide bit on all
  }
  oam_copy(oam_mem, allObjects, 60);

  // hide results backdrop
  turn_off_result_backdrop();

  // reset text
  tte_printf("#{es}");
}

void change_ui_state(ProgramState state) {
  if (state == MENU) {
    // TODO: Turn on the menu background layers that were loaded at boot.
    // unload everything else
    hide_all_objects();
    turn_on_Skiii_logo();

    // unhide "Start" menu option
    (&allObjects[0])->attr0 &= ~(1 << 9); //the 9th bit of OAM is the difference between hidden (10) and regular (00)
    (&allObjects[1])->attr0 &= ~(1 << 9);
    (&allObjects[2])->attr0 &= ~(1 << 9);



    // startup the menu navigation system
    static MenuNode main_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 68, 100, NULL, &main_menu_nodes[1], NULL, NULL, change_state_to_ski },
      { 68, 130, &main_menu_nodes[0], NULL, NULL, NULL, change_state_to_options },
    };
    menu_set_active(&main_menu_nodes[0]);

  } else if (state == OPTIONS) {
    hide_all_objects();
    turn_off_Skiii_logo();
    int hs = read_highscore();

    tte_printf("#{es;P:60,50}HIGH SCORE: %03d", hs);
    tte_printf("#{P:31,115}CREATED BY: TOM RUNNELS", hs);
    tte_printf("#{P:56,135}MUSIC BY: MELLUSI", hs);


    // // startup the menu navigation system
    static MenuNode option_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 255, 255, NULL, NULL, NULL, NULL, change_state_to_menu },
    };
    menu_set_active(&option_menu_nodes[0]);


    // load options screen objects into vram

    // load options screen objects into OAM

    // unload everything else
  } else if (state == SKI) {
    hide_all_objects();
    turn_off_Skiii_logo();

    //unhide the player icon
    (&allObjects[3])->attr0  &= ~(1 << 9);
    
    //unhide the moving flags
    for(int i = 10; i < 20; i++) {
      (&allObjects[i])->attr0  &= ~(1 << 9);
    }

    // TODO: delete.  Debug: select to exit SKI mode
    static MenuNode ski_menu_nodes[3] = {
      // x, y, up, down, left, right, select
      { 255, 255, NULL, NULL, NULL, NULL, change_state_to_menu },
    };
    menu_set_active(&ski_menu_nodes[0]);
    oam_copy(oam_mem, allObjects, 20);

  } else if (state == RESULTS) {
    turn_on_result_backdrop(); 
  }
}
