#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
	//int i;
	//int w = io_read(AM_GPU_CONFIG).width;  // TODO: get the correct width
	//int h = io_read(AM_GPU_CONFIG).height;  // TODO: get the correct height
	//uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	//for (i = 0; i < w * h; i ++) fb[i] = i;
	//outw(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t gpuword = inw(VGACTL_ADDR); 
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = gpuword>>16, .height = gpuword & 0xffff,
    .vmemsz = 0, 
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  //io_write(AM_GPU_FBDRAW, 0, 0, vmem, screen_width(), screen_height(), true)
		int W = inw(VGACTL_ADDR) >> 16;
		int sx = ctl->x, sy = ctl->y, ex = ctl->x + ctl->w, ey = ctl->y + ctl->h;
		uint32_t *fp = (uint32_t*)ctl->pixels;
		for(int i = sy; i < ey; i++){
			for(int j = sx; j < ex; j++){
				outw(FB_ADDR+(i*W+j)*4, *fp);
				fp++;		
			}
		}
	if (ctl->sync) {
		outw(SYNC_ADDR, 1);//mean start sync when the value at SYNC_ADDR is 1
	}
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
