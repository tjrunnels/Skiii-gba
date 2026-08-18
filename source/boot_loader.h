#ifndef BOOT_LOADER_H
#define BOOT_LOADER_H

#include <tonc.h>

typedef struct {
    OBJ_ATTR *flag_icon;
} BootReturn;

BootReturn load_boot_assets(void);
void enable_running_snow_background_0(void);

#endif
