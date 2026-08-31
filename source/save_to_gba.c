#include <tonc.h>
#include "save_to_gba.h"

//TODO: rewrite this cause it's AI

// Magic ID string recognized by GBA emulators (mGBA, VBA) and flashcarts to auto-detect Flash 1M (128 KB)
static const char FLASH_ID[] __attribute__((used, section(".rodata"))) = "FLASH1M_V103";

#define FLASH_BASE       ((vu8*)0x0E000000)
#define FLASH_CMD1       ((vu8*)0x0E005555)
#define FLASH_CMD2       ((vu8*)0x0E002AAA)

#define SAVE_MAGIC       "SKIII"
#define MAGIC_LEN        5

static void flash_set_bank(u8 bank) {
    u16 ime = REG_IME;
    REG_IME = 0;
    *FLASH_CMD1 = 0xAA;
    *FLASH_CMD2 = 0x55;
    *FLASH_CMD1 = 0xB0;
    *FLASH_BASE = bank;
    REG_IME = ime;
}

static int flash_erase_sector(u8 sector) {
    u16 ime = REG_IME;
    REG_IME = 0;
    *FLASH_CMD1 = 0xAA;
    *FLASH_CMD2 = 0x55;
    *FLASH_CMD1 = 0x80;
    *FLASH_CMD1 = 0xAA;
    *FLASH_CMD2 = 0x55;
    *(vu8*)(FLASH_BASE + (sector << 12)) = 0x30;
    REG_IME = ime;

    // Poll until sector erase completes (all bits reset to 0xFF)
    vu8 *sector_ptr = (vu8*)(FLASH_BASE + (sector << 12));
    for (int timeout = 0; timeout < 2000000; timeout++) {
        if (*sector_ptr == 0xFF) {
            return 1;
        }
    }
    return 0;
}

static int flash_write_byte(u32 offset, u8 byte) {
    u16 ime = REG_IME;
    REG_IME = 0;
    *FLASH_CMD1 = 0xAA;
    *FLASH_CMD2 = 0x55;
    *FLASH_CMD1 = 0xA0;
    *(vu8*)(FLASH_BASE + offset) = byte;
    REG_IME = ime;

    // Poll until byte write completes
    vu8 *target = (vu8*)(FLASH_BASE + offset);
    for (int timeout = 0; timeout < 200000; timeout++) {
        if (*target == byte) {
            return 1;
        }
    }
    return 0;
}

void write_highscore(int new_highscore) {
    flash_set_bank(0);
    flash_erase_sector(0);

    // Write magic signature: "SKIII"
    const char magic[] = SAVE_MAGIC;
    for (int i = 0; i < MAGIC_LEN; i++) {
        flash_write_byte(i, (u8)magic[i]);
    }

    // Write 4-byte high score (little-endian)
    flash_write_byte(5, (u8)(new_highscore & 0xFF));
    flash_write_byte(6, (u8)((new_highscore >> 8) & 0xFF));
    flash_write_byte(7, (u8)((new_highscore >> 16) & 0xFF));
    flash_write_byte(8, (u8)((new_highscore >> 24) & 0xFF));
}

void initialize_save_file(void) {
    flash_set_bank(0);

    const char magic[] = SAVE_MAGIC;
    int valid = 1;
    for (int i = 0; i < MAGIC_LEN; i++) {
        if (FLASH_BASE[i] != (u8)magic[i]) {
            valid = 0;
            break;
        }
    }

    if (!valid) {
        write_highscore(0);
    }
}

int read_highscore(void) {
    flash_set_bank(0);

    const char magic[] = SAVE_MAGIC;
    for (int i = 0; i < MAGIC_LEN; i++) {
        if (FLASH_BASE[i] != (u8)magic[i]) {
            return 0;
        }
    }

    int to_return = FLASH_BASE[5] 
                  | (FLASH_BASE[6] << 8) 
                  | (FLASH_BASE[7] << 16) 
                  | (FLASH_BASE[8] << 24);

    if (to_return < 0 || to_return > 999999) {
        to_return = 0;
    }

    return to_return;
}


