#include <stdint.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#include "nuklear.h"
#include "config.h"
#include "debug.h"

typedef char dmesg_t[CONF_DEBUG_MESG_MAX];

static int num_mesg;
static dmesg_t *mesgs;

void debug_init(void)
{
	num_mesg = 0;
	mesgs = malloc(0);
	debugf("Setup Debugger\n");
}

void debugf(const char *fmt, ...) {

	mesgs = realloc(mesgs, sizeof(dmesg_t) * ++num_mesg);

	va_list args;
	va_start(args, fmt);
	vsnprintf(mesgs[num_mesg - 1], CONF_DEBUG_MESG_MAX, fmt, args);
    	va_end(args);
}

void debug_panel(void *ctxin)
{
	struct nk_context *ctx = ctxin;
	static const int width = CONF_WIDTH - 420;
	if(!nk_begin(ctx, "Debug Log", nk_rect(420,
					(CONF_HEIGHT >> 1) * 1.5f, width,
					CONF_HEIGHT >> 2), NK_WINDOW_TITLE |
		NK_WINDOW_BORDER))
		return;

	nk_layout_row_dynamic(ctx, 20, 1);
	for(int i = num_mesg - 1; i >= 0; i--) {
		nk_text_wrap(ctx, mesgs[i], strlen(mesgs[i]) - 1);
	}
	nk_end(ctx);
}
