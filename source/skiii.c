#include "boot_loader.h"
#include "skiii.h"
#include "snow_floor.h"
#include "ui_manager.h"
#include "menu_navigation_manager.h"
#include "mgba_log.h"
#include <tonc.h>
#include "game_logic.h"

// GBA resolution: 240 x 160
ProgramState game_state = MENU;
void set_game_state(ProgramState new_state) {
    game_state = new_state;
}

int main() {
  // Load all assets that have a fixed memory home for the whole program.
  BootReturn bootReturn = load_boot_assets();
  enable_running_snow_background_0();

  // Setup the GBA program
  set_game_state(MENU);
  change_ui_state(game_state);


  
  // Init interrupts and VBlank irq.
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);


  if (dlog_open()) {
    mgbaprintf("mGBA logging ready\n");
  }

  // gameplay variables
  SkiStartAnimation in_ski_start_animation = STANDBY;

  // Scroll around some
  short int SCROLL_DELTA_X = 192/8;
  short int SCROLL_DELTA_Y = 64;

  int game_score = 0;
  int high_score = 0;

  // main game loop
  while (1) {
    // if(game_state == MENU) {
    //   print("state: menu");
    // } else if (game_state == OPTIONS) {
    //   print("state: options");
    // } else if (game_state == SKI) {
    //   print("state: Ski");
    // } else if (game_state == CREDITS) {
    //   print("state: credits");
    // }


    VBlankIntrWait(); //  Wait until the last frame finishes drawing
    key_poll();       //  snapshot of keys i think?

    SCROLL_DELTA_Y += 1;
    // SCROLL_DELTA_X += key_tri_shoulder();

    REG_BG0HOFS = SCROLL_DELTA_X;
    REG_BG0VOFS = SCROLL_DELTA_Y;

    if(game_state != SKI) {
      in_ski_start_animation = STANDBY;
      bootReturn.player_icon->attr0 = (bootReturn.player_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(170);
      game_score = 0;


      //TODO: Delet
      tte_printf("#{es;P:0,0}HIGH SCORE: %04d", high_score);


      // keybindings move the menu navigation system
      if (key_hit(KEY_UP)) {
        menu_nav_up();
      } else if (key_hit(KEY_DOWN)) {
        menu_nav_down();
      } else if (key_hit(KEY_LEFT)) {
        menu_nav_left();
      } else if (key_hit(KEY_RIGHT)) {
        menu_nav_right();
      } else if (key_hit(KEY_A)) {
        menu_select();
      }

      // mgbaprintf("Current menu node: (%d, %d)\n", get_current()->x, get_current()->y);
      bootReturn.flag_icon->attr0 = (bootReturn.flag_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(get_current()->y);
      bootReturn.flag_icon->attr1 = (bootReturn.flag_icon->attr1 & ~ATTR1_X_MASK) | ATTR1_X(get_current()->x);
      oam_copy(oam_mem, allObjects, 4);
    } else if (game_state == SKI) {
      tte_printf("#{es;P:0,0}SCORE: %04d", game_score);

      if(in_ski_start_animation == STANDBY) {
        in_ski_start_animation = BEGIN;
      } 

      // TODO: Delete.  Press A to go back to menu
      if (key_hit(KEY_SELECT)) {
        set_game_state(MENU);
        change_ui_state(MENU);
      }

      if(in_ski_start_animation == BEGIN) {
        print("Beginning!");
        // face forward
        bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex) | ATTR2_PALBANK(2);
        // start player in the middle of the screen
        bootReturn.player_icon->attr1 = (bootReturn.player_icon->attr1 & ~ATTR1_X_MASK) | ATTR1_X(112);
        bootReturn.player_icon->attr0 = (bootReturn.player_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(235);
        in_ski_start_animation = PLAYING;
      } else if(in_ski_start_animation == PLAYING) {
        // move down slowly
        int player_y_value = bootReturn.player_icon->attr0 & 0xFF;
        if(player_y_value < 18 || player_y_value > 160) {
           bootReturn.player_icon->attr0 = (bootReturn.player_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(player_y_value + 1);
        } else {
          //stop at some Y value, stop animation
          in_ski_start_animation = DONE;
          game_setup();
        }
      } else {
        FrameResult res = process_game_frame(bootReturn);
        if(res == GAME_OVER) {
          set_game_state(MENU);
          change_ui_state(MENU);
        } else if (res == SCORED_POINT) {
          game_score++;
          if(game_score > high_score) high_score = game_score;
        }
      }


      oam_copy(oam_mem, allObjects, 20);
    }
  }

  return 0;
}
