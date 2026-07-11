#ifndef MGBA_LOG_H
#define MGBA_LOG_H

#include <stdbool.h>

/* mGBA debug-register logging. View output with Tools → View logs… */

bool dlog_open(void);
void mgbaprintf(const char *fmt, ...);

#endif
