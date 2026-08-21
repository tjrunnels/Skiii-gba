#ifndef BOOT_LOADER_H
#define BOOT_LOADER_H

#include <tonc.h>

typedef struct {
    OBJ_ATTR *start_icon;
    OBJ_ATTR *options_icon;
    OBJ_ATTR *flag_icon;
} BootReturn;

BootReturn load_boot_assets(void);
void enable_running_snow_background_0(void);

extern OBJ_ATTR allObjects[128];

#endif
