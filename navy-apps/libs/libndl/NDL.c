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
static int screen_w = 0, screen_h = 0;//屏幕
static int canvas_w = 0, canvas_h = 0;//画布
static int offset_w = 0, offset_h = 0;
static uint32_t *canvas = NULL;


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
	 
	/* 
	FILE* fp = fopen("/dev/events","r");
	memset(buf, 0, len);
	int ret = fread(buf, 1, len, fp);
	fclose(fp);
	if(ret == 0)return 0;
	for(int i = 0;i<len;i++)
		if(buf[i]=='\n'){
			buf[i]='\0';
			break;
		}
	return ret;
*/
}

void NDL_OpenCanvas(int *w, int *h) {
	//读取屏幕大小
	FILE* fp = fopen("/proc/dispinfo", "r");
	assert(fp);
	fscanf(fp, "WIDTH:%d HEIGHT:%d", &screen_w, &screen_h);
    //printf("%d %d\n", screen_w, screen_h);
	
	//设定画布大小
	assert(*w>=0 && *h>=0);
	canvas_w = *w < screen_w ? *w : screen_w;
	canvas_h = *h < screen_h ? *h : screen_h;
	if(*w==0) canvas_w = screen_w;
	if(*h==0) canvas_h = screen_h;

	//打开画布
	canvas = (uint32_t*)malloc(canvas_w * canvas_h * sizeof(uint32_t));
	memset(canvas, 0, sizeof(canvas));
	//printf("%d %d\n", canvas_w, canvas_h);

	offset_w = (screen_w - canvas_w)/2;//中间
    offset_h = (screen_h - canvas_h)/2;

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
	
	FILE* fp = fopen("/dev/fb", "w");
	assert(fp);
	if(h == 0||h > canvas_h)h = canvas_h;
	if(w == 0||w > canvas_w)w = canvas_w;


	for(int i = 0; i<h; i++){//复制到画布上
		for(int j = 0; j<w; j++){
			canvas[canvas_w*(y+i)+x+j] = pixels[i*w+j];
		}
	}

	for(int i = 0; i<canvas_h; i++){//从画布复制到/dev/fb,这里可以将画布放到任意位置,此时是左上角
		fseek(fp, (offset_w + screen_w * (offset_h + i))*sizeof(uint32_t), SEEK_SET);
		fwrite((canvas+canvas_w*i), sizeof(uint32_t),  canvas_w, fp);
	}
	return;
	

	/*
	int fd = open("/dev/fb", O_RDWR);
	assert(fd);

	for (int i = 0; i < h; ++i){
		lseek(fd, ((y + i) * screen_w + x) * sizeof(uint32_t), SEEK_SET);
		ssize_t s = write(fd, pixels + w * i, w * sizeof(uint32_t));
	}
	*/
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
