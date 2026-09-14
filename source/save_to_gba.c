#include <tonc.h>
#include "save_to_gba.h"

// Magic ID string recognized by GBA emulators (mGBA, VBA) and flashcarts to auto-detect Flash 1M (128 KB)
static const char FLASH_ID[] __attribute__((used, section(".rodata"))) = "FLASH1M_V103";


// Addreses for the different sequences you have to do to write to Flash 
#define FLASH_BASE       ((vu8*)0x0E000000)
#define FLASH_CMD1       ((vu8*)0x0E005555)
#define FLASH_CMD2       ((vu8*)0x0E002AAA)

// Our magic string to know if this is a fresh game file (no save initialized yet) 
#define SAVE_MAGIC       "SKIII"
#define MAGIC_LEN        5


// There are two halves of our memory card... we only need to use one  
static void set_bank_to_0() {
    u16 ime = REG_IME; 
    REG_IME = 0; // block interupts
    
    *FLASH_CMD1 = 0xAA; *FLASH_CMD2 = 0x55; // sequence start
    *FLASH_CMD1 = 0xB0; // bank switch sequence
    *FLASH_BASE = 0;
    
    REG_IME = ime; // unblock interupts
}

static int flash_erase_sector(u8 sector) {
    u16 ime = REG_IME;
    REG_IME = 0; // block interupts
    
    *FLASH_CMD1 = 0xAA; *FLASH_CMD2 = 0x55; // sequence start
    *FLASH_CMD1 = 0x80; *FLASH_CMD1 = 0xAA; *FLASH_CMD2 = 0x55; // erase sequence
    *(vu8*)(FLASH_BASE + (sector << 12)) = 0x30;
    
    REG_IME = ime; // unblock interupts

    // Poll to make sure sector erase completes 
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
    REG_IME = 0; // block interupts
    
    *FLASH_CMD1 = 0xAA; *FLASH_CMD2 = 0x55; // sequence starts
    *FLASH_CMD1 = 0xA0; // write byte sequence
    *(vu8*)(FLASH_BASE + offset) = byte;
    
    REG_IME = ime; // unblock interupts

    // Poll to make sure byte write completes
    vu8 *target = (vu8*)(FLASH_BASE + offset);
    for (int timeout = 0; timeout < 200000; timeout++) {
        if (*target == byte) {
            return 1;
        }
    }
    return 0;
}

void write_highscore(int new_highscore) {
    set_bank_to_0();
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
    set_bank_to_0();

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
    set_bank_to_0();

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


