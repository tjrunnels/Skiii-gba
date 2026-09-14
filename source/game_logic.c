#include <tonc.h>
#include "boot_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_logic.h"
#include <maxmod.h>
#include "soundbank.h"


static int SPEED = 2;
static int x_velocity = 0; 

static int next_flag_group = 0;
static int frame_countdown_to_next_flag = 60;
static int frames_between_flags = 60;

typedef enum { LEFT, RIGHT, NONE } InputBuffer;
InputBuffer input_buffer = NONE;
const int INPUT_BUFFER_SIZE = 19;


static int speed_up_delay_counter = 0;
// is called every time a new flag group is spawned 
void speed_up_logic(void) {
  if(frames_between_flags > 45) {
    frames_between_flags--;
  } else if (frames_between_flags > 40) {
    if(++speed_up_delay_counter == 2) { // every other time
      speed_up_delay_counter = 0;
      frames_between_flags--;
    }
  } else if (frames_between_flags > 35) {
    if(++speed_up_delay_counter == 3) { // every third time
      speed_up_delay_counter = 0;
      frames_between_flags--;
    }
  } else if (frames_between_flags > 32) {
    if(++speed_up_delay_counter == 4) { // every fourth time
      speed_up_delay_counter = 0;
      frames_between_flags--;
    }
  } 
  // else, do nothing.  32 will be the max
}

void game_setup(int frame_count) {
  next_flag_group = 0;
  frame_countdown_to_next_flag = 60;
  frames_between_flags = 60;
  speed_up_delay_counter = 0;
  input_buffer = NONE;
  srand(frame_count ^ REG_VCOUNT); // frame count mixed with current line being drawn... seems pretty random to me
  
  //spawn initial 7 flags
  for(int i = 0; i < 7; i++) {
    OBJ_ATTR *next_flag_left_init = &allObjects[(next_flag_group*2) + 10];
    OBJ_ATTR *next_flag_right_init = &allObjects[(next_flag_group*2) + 11];
    
    if(++next_flag_group == 7) {
      // loop 
      next_flag_group = 0;
    }

    const short int lane = rand() % 3;

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
  
  int player_x_value = bootReturn.player_icon->attr1 & 0xFF;

  // if x = {lane 1} || x = {lane 2} || x = {lane 3}, set X velocity to 0 and straight sprite
  if(x_velocity != 0 && (player_x_value == 32 || player_x_value == 112 || player_x_value == 192)) {
    x_velocity = 0;

    // set the sprite id to base (facing straight)
    bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex) | ATTR2_PALBANK(2) | ATTR2_PRIO(1);
  }


    if (key_hit(KEY_LEFT) || key_hit(KEY_L) || (x_velocity == 0 && input_buffer == LEFT)) {
      // reset input buffer 
      input_buffer = NONE;

      // left is touched while headed left and within a few pixels of the lane, buffer the input
      if(x_velocity < 0 && 
          ((player_x_value > 32 && player_x_value < (32 + INPUT_BUFFER_SIZE))
       || (player_x_value > 112 && player_x_value < (112 + INPUT_BUFFER_SIZE))
       || (player_x_value > 192 && player_x_value < (192 + INPUT_BUFFER_SIZE)))
      ) {
        input_buffer = LEFT;
      }
      else { // set X velocity to negative, left sprite
        mmEffect(SFX_TURN);
        x_velocity = -1 * SPEED;
        
        // set the sprite id to base+4
        bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex + 4) | ATTR2_PALBANK(2) | ATTR2_PRIO(1);
      }        
    } 
    else if (key_hit(KEY_RIGHT) || key_hit(KEY_R) || (x_velocity == 0 && input_buffer == RIGHT)) {
      // reset input buffer 
      input_buffer = NONE;

      // right is touched while headed right and within a few pixels of the lane, buffer the input
      if(x_velocity > 0 && 
          ((player_x_value < 32 && player_x_value > (32 - INPUT_BUFFER_SIZE))
       || (player_x_value < 112 && player_x_value > (112 - INPUT_BUFFER_SIZE))
       || (player_x_value < 192 && player_x_value > (192 - INPUT_BUFFER_SIZE)))
      ) {
        input_buffer = RIGHT;
      }
      else { // set X velocity to negative, right sprite
        mmEffect(SFX_TURN);
        x_velocity = 1 * SPEED;
      
        // set the sprite id to base-4
        bootReturn.player_icon->attr2 = ATTR2_ID(bootReturn.playerImageBaseIndex - 4) | ATTR2_PALBANK(2) | ATTR2_PRIO(1);
      }
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
      
      if(++next_flag_group == 7) {
        // loop 
        next_flag_group = 0;
      }

      const short int lane = (rand() % 3);

      if(lane == 0) {
        next_flag_left->attr1 = (next_flag_left->attr1 & ~ATTR1_X_MASK) | ATTR1_X(2);  //2
        next_flag_right->attr1 = (next_flag_right->attr1 & ~ATTR1_X_MASK) | ATTR1_X(62); //62
      } else if(lane == 1) {
        next_flag_left->attr1 = (next_flag_left->attr1 & ~ATTR1_X_MASK) | ATTR1_X(82); //82
        next_flag_right->attr1 = (next_flag_right->attr1 & ~ATTR1_X_MASK) | ATTR1_X(142); //142
      } else if(lane == 2) {
        next_flag_left->attr1 = (next_flag_left->attr1 & ~ATTR1_X_MASK) | ATTR1_X(162); //162
        next_flag_right->attr1 = (next_flag_right->attr1 & ~ATTR1_X_MASK) | ATTR1_X(222); //222
      }
    
      //set y
      next_flag_left->attr0 = (next_flag_left->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(160); // right at screen bottom (in increment range)
      next_flag_right->attr0 = (next_flag_right->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(160);

      speed_up_logic();
    }

    // each flag group
    for (int i = 0; i < 7; i++) {
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

        OBJ_ATTR *flagR = &allObjects[(i*2) + 11];
        flagL->attr0 = (flagL->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(new_y_value);
        flagR->attr0 = (flagR->attr0 & ~ATTR0_Y_MASK) | ATTR0_Y(new_y_value);
      }
    }    
    oam_copy(oam_mem, allObjects, 24);
    return to_return;
};