#include "mgba_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <tonc.h>

/* mGBA-only memory-mapped debug registers (no-ops on real hardware). */
#define REG_DEBUG_ENABLE (*(vu16 *)0x4FFF780)
#define REG_DEBUG_FLAGS  (*(vu16 *)0x4FFF700)
#define REG_DEBUG_STRING ((char *)0x4FFF600)

#define MGBA_LOG_DEBUG 4
#define MGBA_LOG_SEND  0x100

bool dlog_open(void) {
  REG_DEBUG_ENABLE = 0xC0DE;
  /* mGBA echoes 0x1DEA when logging is available. */
  return REG_DEBUG_ENABLE == 0x1DEA;
}

void mgbaprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(REG_DEBUG_STRING, 0x100, fmt, args);
  va_end(args);
  REG_DEBUG_FLAGS = MGBA_LOG_DEBUG | MGBA_LOG_SEND;
}
