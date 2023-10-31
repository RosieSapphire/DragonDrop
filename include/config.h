#ifndef CONFIG_H_
#define CONFIG_H_

#define CONF_WIDTH 1920
#define CONF_HEIGHT 1080
#define CONF_ASPECT ((float)CONF_WIDTH / (float)CONF_HEIGHT)
#define CONF_NAME "United64"

#define CONF_NAME_MAX 128
#define CONF_DEBUG_MESG_MAX 512

#define CONF_NEAR 0.1f
#define CONF_FAR 64.0f

#define CONF_VERT_BUF_MAX 512 * 1024
#define CONF_ELEM_BUF_MAX 128 * 1024
#define CONF_LOAD_BUF_MAX 128

#endif /* CONFIG_H_ */
