#ifndef BOOT_LOADER_H
#define BOOT_LOADER_H

#include <tonc.h>

typedef struct {
    OBJ_ATTR *start_icon;
    OBJ_ATTR *options_icon;
    OBJ_ATTR *flag_icon;
    OBJ_ATTR *player_icon;
    int playerImageBaseIndex;
} BootReturn;

BootReturn load_sprites(void);
void load_backgrounds(void);

extern OBJ_ATTR allObjects[128];

#endif
