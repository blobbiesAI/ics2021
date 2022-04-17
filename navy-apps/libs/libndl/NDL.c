#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
	struct timeval timer;
	gettimeofday(&timer, NULL);//用户程序中所使用的函数都是真实计算机内建的函数。
	return timer.tv_sec;       //man 2 gettimeofday
}


int NDL_PollEvent(char *buf, int len) {
	 int fd = open("/dev/events", O_RDONLY);
	 if(fd==-1){return 0;}
	 read(fd, buf, len);
	 return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
	char str[128]; int count = 30;
	int fd = open("/proc/dispinfo", O_RDONLY);
	assert(fd!=-1);
	read(fd, str, count);
	char a[10], b[10];
	sscanf(str, "%[a-z:]%d %[a-z:]%d", a, &screen_w, b, &screen_h);
    printf("%d %d\n", screen_w, screen_h);


  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
	screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
