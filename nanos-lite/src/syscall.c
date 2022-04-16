#include <common.h>
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
void sys_write(Context*);
void sys_brk(Context*);


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	//printf("a[0]:GPR1=%d\n",a[0]);
  switch (a[0]) { 
	case SYS_exit   : sys_exit(c);  break; 
	case SYS_yield  : sys_yield(c); break;
	case SYS_write  : sys_write(c); break;
	case SYS_brk    : sys_brk(c);   break;
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

void sys_write(Context *c){
	uintptr_t fd = c->GPR2;
	uintptr_t buff = c->GPR3;
	uintptr_t count = c->GPR4;

	size_t i = 0;
	char* p = (char*)buff;
	if(fd==1 || fd==2){
		for(; i < count; i++){
			putch(*p);
			p++;
		}
		c->GPRx = i;
	}
	else{	
		c->GPRx = -1;
	}
	return;
}


void sys_brk(Context *c){
	c->GPRx = 0;
	return;
}

