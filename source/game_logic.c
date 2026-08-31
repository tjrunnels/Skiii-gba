#include <tonc.h>
#include "mgba_log.h"
#include "boot_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_logic.h"


static void print(char message[20]) {
  mgbaprintf(message);
}

static int SPEED = 2;
static int x_velocity = 0; 
static int y_velocity = 1;
// static int canMove = 0;

static int next_flag_group = 0;
static int frame_countdown_to_next_flag = 60;
static int frames_between_flags = 60;


void game_setup(void) {
  srand(time(NULL));
  
  //spawn initial 5 flags
  for(int i = 0; i < 5; i++) {
    OBJ_ATTR *next_flag_left_init = &allObjects[(next_flag_group*2) + 10];
    OBJ_ATTR *next_flag_right_init = &allObjects[(next_flag_group*2) + 11];
    
    if(++next_flag_group == 5) {
      // loop 
      next_flag_group = 0;
    }

    const short int lane = rand() % 3;
    char lane_str[16];
    snprintf(lane_str, sizeof(lane_str), "%d", lane);
    print(lane_str);
    print("------");

    // set x
    if(lane == 0) {
        next_flag_left_init->attr1 = (next_flag_left_init->attr1 & ~ATTR1_X_MASK) | ATTR1_X(2);  //2
        next_flag_right_init->attr1 = (next_flag_right_init->attr1 & ~ATTR1_X_MASK) | ATTR1_X(62); //62
    } else if(lane == 1) {
        next_flag_left_init->attr1 = (next_flag_left_init->attr1 & ~ATTR1_X_MASK) | ATTR1_X(72); //72
        next_flag_right_init->attr1 = (next_flag_right_init->attr1 & ~ATTR1_X_MASK) | ATTR1_X(142); //142
    } else if(lane == 2) {
        next_flag_left_init->attr1 = (next_flag_left_init->attr1 & ~ATTR1_X_MASK) | ATTR1_X(162); //162
        next_flag_right_init->attr1 = (next_flag_right_init->attr1 & ~ATTR1_X_MASK) | ATTR1_X(222); //222
    } else {
      SPEED = -1 * SPEED; //if we start reversing, we'll know theres a bug haha //TODO: delete
    }

    //set y
    next_flag_left_init->attr0 = (next_flag_left_init->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(220); //somewhere offscreen
    next_flag_right_init->attr0 = (next_flag_right_init->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(220);
    //unhide
    next_flag_left_init->attr0 &= ~(1 << 9);
    next_flag_right_init->attr0 &= ~(1 << 9);

  }
    

}



// only runs NOT during opening animation
FrameResult process_game_frame(BootReturn bootReturn) {
    FrameResult to_return = NOTHING;

    //   see C:\Users\super\Documents\Development\Skiii\Assets\customassets\Scripts\movement.cs
    int player_x_value = bootReturn.player_icon->attr1 & 0xFF;
    
    // char player_x_str[16];
    // snprintf(player_x_str, sizeof(player_x_str), "%d", player_x_value);

    // print(player_x_str);


    // if x = {lane 1} || x = {lane 2} || x = {lane 3}, set X velocity to 0 and straight sprite
    if(x_velocity != 0 && (player_x_value == 32 || player_x_value == 112 || player_x_value == 192)) {
      x_velocity = 0;

      // set the sprite id to base (facing straight)
      bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex) | ATTR2_PALBANK(2);
    }

    // if left is touched, set X velocity to negative, left sprite
    if (key_hit(KEY_LEFT) || key_hit(KEY_L)) {
      x_velocity = -1 * SPEED;
      
      // set the sprite id to base+4
      bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex + 4) | ATTR2_PALBANK(2);
        
    } 
    // if right, set X velocity to negative, right sprite
    else if (key_hit(KEY_RIGHT) || key_hit(KEY_R)) {
      x_velocity = 1 * SPEED;
      
      // set the sprite id to base-4
      bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex - 4) | ATTR2_PALBANK(2);
    }

    // if x is {at the edges}, teleport & keep velocity
    if (player_x_value == 0) player_x_value = 226; else if (player_x_value == 226) player_x_value = 0;
    
    // apply x velocity
    bootReturn.player_icon->attr1 = (bootReturn.player_icon->attr1 & ~ATTR1_X_MASK) | ATTR1_X(player_x_value + x_velocity);
    
    // ---------------------------
    // respawn next flags

    
    if(--frame_countdown_to_next_flag == 0) {
      frame_countdown_to_next_flag = frames_between_flags;

      OBJ_ATTR *next_flag_left = &allObjects[(next_flag_group*2) + 10];
      OBJ_ATTR *next_flag_right = &allObjects[(next_flag_group*2) + 11];
      
      if(++next_flag_group == 5) {
        // loop 
        next_flag_group = 0;
      }

      const short int lane = (rand() % 3); // * 2; //0 or 2
      char lane_str[16];
      snprintf(lane_str, sizeof(lane_str), "%d", next_flag_group * 1000);
      print(lane_str);


      if(lane == 0) {
        next_flag_left->attr1 = (next_flag_left->attr1 & ~ATTR1_X_MASK) | ATTR1_X(2);  //2
        next_flag_right->attr1 = (next_flag_right->attr1 & ~ATTR1_X_MASK) | ATTR1_X(62); //62
      } else if(lane == 1) {
        next_flag_left->attr1 = (next_flag_left->attr1 & ~ATTR1_X_MASK) | ATTR1_X(82); //82
        next_flag_right->attr1 = (next_flag_right->attr1 & ~ATTR1_X_MASK) | ATTR1_X(142); //142
      } else if(lane == 2) {
        next_flag_left->attr1 = (next_flag_left->attr1 & ~ATTR1_X_MASK) | ATTR1_X(162); //162
        next_flag_right->attr1 = (next_flag_right->attr1 & ~ATTR1_X_MASK) | ATTR1_X(222); //222
      } else {
        SPEED = -1 * SPEED; //if we start reversing, we'll know theres a bug haha //TODO: delete
      }
    
      //set y
      next_flag_left->attr0 = (next_flag_left->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(160); // right at screen bottom (in increment range)
      next_flag_right->attr0 = (next_flag_right->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(160);


      //speed up script 
      frames_between_flags = frames_between_flags > 1 ? frames_between_flags - 1 : frames_between_flags;
    }

    // each flag group
    for (int i = 0; i < 5; i++) {
      OBJ_ATTR *flagL = &allObjects[(i*2) + 10];
      int flagL_y_value = flagL->attr0 & ATTR0_Y_MASK;

      // 1. Collision logic
      const int player_hitbox_y = 18 + 16; //bottom edge of player
      const int flag_hitbox_y = flagL_y_value + 16; //bottom edge of flag
      
      // this should only happen for one flag group on any given frame
      if(flag_hitbox_y == player_hitbox_y) { //TODO: make this a range?
        const int flagL_x_value = flagL->attr1 & ATTR1_X_MASK;

        //TODO: maybe give a buffer? 
        // if player's x is within the two flags' x
        if(player_x_value > flagL_x_value && player_x_value < flagL_x_value + 60) {
          to_return = SCORED_POINT;
        } else {
          to_return = GAME_OVER;
        }
      }


      // 2. else, apply y velocity to all flags that are on the screen
      // two valid zones: less that 160 (on screen) and 240 thru 255 (off the edge of the top of the screen)
      if(!(flagL_y_value > 161 && flagL_y_value < 230)) {
        const int new_y_value = (flagL_y_value - 1) & 0xFF; //bitwise mask will cause it to warp back around to 255

        if(flagL_y_value < 5) {
            char lane_str[16];
            snprintf(lane_str, sizeof(lane_str), "%d", new_y_value);
            print(lane_str);
        }

        OBJ_ATTR *flagR = &allObjects[(i*2) + 11];
        flagL->attr0 = (flagL->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(new_y_value);
        flagR->attr0 = (flagR->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(new_y_value);
      }
    }    
    oam_copy(oam_mem, allObjects, 20);
    return to_return;
};