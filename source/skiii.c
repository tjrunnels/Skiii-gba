#include "boot_loader.h"
#include "skiii.h"
#include "snow_floor.h"
#include "ui_manager.h"
#include "menu_navigation_manager.h"
#include "mgba_log.h"
#include <tonc.h>
#include "game_logic.h"
#include "save_to_gba.h"
#include "music.h"

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

// GBA resolution: 240 x 160
ProgramState game_state = MENU;
void set_game_state(ProgramState new_state) {
    game_state = new_state;
};


void turn_on_results_backdrop() {
  REG_DISPCNT = DCNT_BG3 | REG_DISPCNT; //turn on the index 9 bit
}
void turn_off_results_backdrop() {
  REG_DISPCNT = ~(DCNT_BG3 | ~REG_DISPCNT); //turn off the index 9 bit
}


int main() {
  // Load all assets that have a fixed memory home for the whole program.
  initialize_save_file();
  BootReturn bootReturn = load_sprites();
  load_backgrounds();

  // Setup the GBA program
  set_game_state(MENU);
  change_ui_state(game_state);


  
  // Init interrupts and VBlank irq.
  // NOTE: mmVBlank MUST be linked to VBlank IRQ - it feeds MaxMod's
  // DirectSound mixer every frame. With NULL here, mmEffect() queues
  // but nothing ever outputs, so you get silence.
  irq_init(NULL);
  irq_add(II_VBLANK, mmVBlank);

  // init sfx engine (sets up sound regs itself, don't overwrite after)
  mmInitDefault((mm_addr)soundbank_bin, 8);
  mmSetEffectsVolume(1024); // max SFX volume (0-1024 = 0%-100%)

  if (dlog_open()) {
    mgbaprintf("mGBA logging ready\n");
  }

  // gameplay variables
  SkiStartAnimation in_ski_start_animation = STANDBY;
  ResultScreenAnimation in_result_animation = STANDBY;

  // Scroll around some
  short int SCROLL_DELTA_X = 192/8;
  short int SCROLL_DELTA_Y = 64;
  int RESULT_SCREEN_Y = 0;

  int game_score = 0;
  int high_score = read_highscore();

  int frame_count = 0;

  init_music();   

  // main game loop
  while (1) {
    VBlankIntrWait();
    mmFrame();
    loop_music_frame();
    frame_count++;
    // if(game_state == MENU) {
    //   print("state: menu");
    // } else if (game_state == OPTIONS) {
    //   print("state: options");
    // } else if (game_state == SKI) {
    //   print("state: Ski");
    // } else if (game_state == CREDITS) {
    //   print("state: credits");
    // }


    key_poll();       //  snapshot of keys i think?

    SCROLL_DELTA_Y += 1;
    // SCROLL_DELTA_X += key_tri_shoulder();

    REG_BG0HOFS = SCROLL_DELTA_X;
    REG_BG0VOFS = SCROLL_DELTA_Y;

    if(game_state != SKI && game_state != RESULTS) {
      in_ski_start_animation = STANDBY;
      in_result_animation = _STANDBY;

      // hide skier
      bootReturn.player_icon->attr0 = (bootReturn.player_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(235);

      // hide flags
      game_setup(frame_count);


      // keybindings move the menu navigation system
      if (key_hit(KEY_UP)) {
        menu_nav_up();
      } else if (key_hit(KEY_DOWN)) {
        menu_nav_down();
      } else if (key_hit(KEY_A)) {
        menu_select();
      }

      // mgbaprintf("Current menu node: (%d, %d)\n", get_current()->x, get_current()->y);
      bootReturn.flag_icon->attr0 = (bootReturn.flag_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(get_current()->y);
      bootReturn.flag_icon->attr1 = (bootReturn.flag_icon->attr1 & ~ATTR1_X_MASK) | ATTR1_X(get_current()->x);
      oam_copy(oam_mem, allObjects, 4);
    } else if (game_state == SKI) {
      tte_printf("#{es;P:203,8}%03d", game_score);

      if(in_ski_start_animation == STANDBY) {
        in_ski_start_animation = BEGIN;
      } 

      if(in_ski_start_animation == BEGIN) {
        // reset text bg location      
        REG_BG2VOFS = 0;
        print("Beginning!");
        
        // face forward
        bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex) | ATTR2_PALBANK(2) | ATTR2_PRIO(1);
       
        // start player in the middle of the screen
        bootReturn.player_icon->attr1 = (bootReturn.player_icon->attr1 & ~ATTR1_X_MASK) | ATTR1_X(112);
        bootReturn.player_icon->attr0 = (bootReturn.player_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(235);
        in_ski_start_animation = PLAYING;

        // reset score
        game_score = 0;
        // reset flags
        game_setup(frame_count);

      } else if(in_ski_start_animation == PLAYING) {
        // move down slowly
        int player_y_value = bootReturn.player_icon->attr0 & 0xFF;
        if(player_y_value < 18 || player_y_value > 160) {
          if(player_y_value + 1 == 256) {
            player_y_value = 0;
          }
          bootReturn.player_icon->attr0 = (bootReturn.player_icon->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(player_y_value + 1);
        } else {
          //stop at some Y value, stop animation
          in_ski_start_animation = DONE;
        }
      } else {
        FrameResult res = process_game_frame(bootReturn);
        if(res == GAME_OVER) {
          set_game_state(RESULTS);
          change_ui_state(RESULTS);
        } else if (res == SCORED_POINT) {
          game_score++;
          if(game_score % 10 == 0) {
            mmEffect(SFX_XPOINTS);
          } else {
            mmEffect(SFX_POINT);
          }
          if(game_score > high_score) {
            high_score = game_score;
            write_highscore(high_score);
          }
        }
      }


      oam_copy(oam_mem, allObjects, 20);
    } else if (game_state == RESULTS) {
      in_ski_start_animation = _STANDBY;

      if(in_result_animation == _STANDBY) {
        // reset Y location
        RESULT_SCREEN_Y = 0;
        in_result_animation = _BEGIN;
      }

      if(in_result_animation == _BEGIN) {
        // set bg3 (overlay) and bg2 (text) location to offscreen
        REG_BG3VOFS = RESULT_SCREEN_Y;
        REG_BG2VOFS = RESULT_SCREEN_Y + 100;
        
        // turn on bg3
        turn_on_results_backdrop();

        in_result_animation = _PLAYING;

        tte_printf("#{es;P:83,20}Score: %03d", game_score);
        tte_printf("#{P:63,40}High Score: %03d", read_highscore());
        tte_printf("#{P:79,80}A: Try again");
        tte_printf("#{P:63,100}B: Exit to menu");

      } else if (in_result_animation == _PLAYING) {
        if(RESULT_SCREEN_Y < 148) {
          RESULT_SCREEN_Y += 2;
        } else if (RESULT_SCREEN_Y == 148) {
          in_result_animation = DONE;
        }
        REG_BG3VOFS = RESULT_SCREEN_Y;
        REG_BG2VOFS = RESULT_SCREEN_Y + 100;
      }

      // input options
      if (key_hit(KEY_A)) {
        in_result_animation = _STANDBY;
        set_game_state(SKI);
        change_ui_state(SKI);
      } else if (key_hit(KEY_B)) {
        in_result_animation = _STANDBY;
        set_game_state(MENU);
        change_ui_state(MENU);
      }

    }
  }

  return 0;
}
