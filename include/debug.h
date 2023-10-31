#ifndef DEBUG_H_
#define DEBUG_H_

#include "config.h"

typedef char dmesg_t[CONF_DEBUG_MESG_MAX];

void debug_init(void);
void debugf(const char *fmt, ...);
void debug_panel(void *ctx);

#endif /* DEBUG_H_ */
