#include <tonc.h>
#include "mgba_log.h"
#include "boot_loader.h"
#include <stdio.h>

static void print(char message[20]) {
  mgbaprintf(message);
}

static const int SPEED = 2;
static int x_velocity = 0; 
// static int canMove = 0;

// only runs NOT during opening animation
void process_game_frame(BootReturn bootReturn) {

    //   see C:\Users\super\Documents\Development\Skiii\Assets\customassets\Scripts\movement.cs
    int player_x_value = bootReturn.player_icon->attr1 & 0xFF;
    
    // char player_x_str[16];
    // snprintf(player_x_str, sizeof(player_x_str), "%d", player_x_value);

    // print(player_x_str);


    // if x = {lane 1} || x = {lane 2} || x = {lane 3}, set X velocity to 0 and straight sprite
    if(x_velocity != 0 && (player_x_value == 52 || player_x_value == 112 || player_x_value == 172)) {
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
    
    // apply velocity
    bootReturn.player_icon->attr1 = (bootReturn.player_icon->attr1 & ~ATTR1_X_MASK) | ATTR1_X(player_x_value + x_velocity);
    oam_copy(oam_mem, allObjects, 4);
};