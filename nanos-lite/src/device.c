#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	char *p = (char*)buf;
	size_t i = 0;
	for(; i < len; i++){
		putch(*p);
		p++;
	}	
	return i;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	AM_INPUT_KEYBRD_T keyboard = io_read(AM_INPUT_KEYBRD);
	if(keyboard.keycode == AM_KEY_NONE){return 0;}

	if(keyboard.keydown){
		return snprintf((char*)buf, len, "kd %s", keyname[keyboard.keycode]);
	}
	return snprintf((char*)buf, len, "ku %s", keyname[keyboard.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	AM_GPU_CONFIG_T screen = io_read(AM_GPU_CONFIG);
	snprintf((char*)buf, len, "WIDTH:%d HEIGHT:%d", screen.width, screen.height);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
