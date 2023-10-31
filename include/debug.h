#ifndef DEBUG_H_
#define DEBUG_H_

void debug_init(void);
void debugf(const char *fmt, ...);
void debug_panel(void *ctx);

#endif /* DEBUG_H_ */
