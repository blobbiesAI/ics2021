#include <common.h>
#include <fs.h>
#include <sys/time.h>

enum {
   SYS_exit,
   SYS_yield,
   SYS_open,
   SYS_read,
   SYS_write,
   SYS_kill,
   SYS_getpid,
   SYS_close,
   SYS_lseek,
   SYS_brk,
   SYS_fstat,
   SYS_time,
   SYS_signal,
   SYS_execve,
   SYS_fork,
   SYS_link,
   SYS_unlink,
   SYS_wait,
   SYS_times,
   SYS_gettimeofday
 };



void sys_exit(Context*);
void sys_yield(Context*);
void sys_open(Context*);
void sys_read(Context*);
void sys_write(Context*);
void sys_close(Context*);
void sys_lseek(Context*);
void sys_brk(Context*);
void sys_execve(Context*);
void sys_gettimeofday(Context*);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	//printf("a[0]:GPR1=%d\n",a[0]);
  switch (a[0]) { 
	case SYS_exit   : sys_exit(c);  break; 
	case SYS_yield  : sys_yield(c); break;
	case SYS_open   : sys_open(c);  break;
	case SYS_read   : sys_read(c);  break;
	case SYS_write  : sys_write(c); break;
	case SYS_close  : sys_close(c); break;
	case SYS_lseek  : sys_lseek(c); break;
	case SYS_brk    : sys_brk(c);   break;
	case SYS_execve : sys_execve(c);break;
	case SYS_gettimeofday: sys_gettimeofday(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  } 
}

void sys_yield(Context *c){
	yield();
	c->GPRx = 0;
	return;
}

void sys_exit(Context *c){
	halt(0);
	c->GPRx = 0;
	return;
}

void sys_open(Context *c){
	char* filename = (char*)c->GPR2;
	int flags = c->GPR3;
	int mode = c->GPR4;
	int fd = fs_open(filename, flags, mode);
	c->GPRx = fd;
	return;
}

void sys_read(Context* c){ 
	uintptr_t fd = c->GPR2;
	uintptr_t buff = c->GPR3;
	uintptr_t count = c->GPR4;

	if(fd==1 || fd==2){
		c->GPRx = -1;
	} 
	else{
		c->GPRx = fs_read(fd, (void*)buff, count);
	}
	return;

}


void sys_write(Context *c){
	uintptr_t fd = c->GPR2;
	uintptr_t buff = c->GPR3;
	uintptr_t count = c->GPR4;

	//size_t i = 0;
	//char* p = (char*)buff;
	//if(fd==1 || fd==2){//已经添加了虚拟文件系统，统一到fs_read
	//	for(; i < count; i++){
	//		putch(*p);
	//		p++;
	// 	}
	//	c->GPRx = i;
	//} 
	//else{	
		c->GPRx = fs_write(fd, (void*)buff, count);
	//}
	return;
}

void sys_close(Context* c){
	int fd = c->GPR2;
	c->GPRx = fs_close(fd);
	return;
}


void sys_lseek(Context *c){
	int fd = c->GPR2;
	size_t offset = c->GPR3;
	int whence = c->GPR4;
	c->GPRx = fs_lseek(fd, offset, whence);
	return;
}


void sys_brk(Context *c){
	c->GPRx = 0;
	return;
}


int pexecve(const char* filename, char *const argv[], char *const envp[]);
void sys_execve(Context *c){
	char* filename = (char*)c->GPR2;
	char ** argv = (char**)c->GPR3;
	char ** envp = (char**)c->GPR4;

	c->GPRx = pexecve(filename, argv, envp);	

	return;
}



void sys_gettimeofday(Context *c){
	struct timeval *timer = (struct timeval*)c->GPR2;
	uint64_t sec = io_read(AM_TIMER_UPTIME).us;
	timer->tv_sec = sec/1000000;
	timer->tv_usec = sec%1000000;
	c->GPRx = 0;
	return;
}
