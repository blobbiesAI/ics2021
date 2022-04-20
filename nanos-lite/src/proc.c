#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}


void context_kload(PCB *pcb, void (*entry)(void *), void *arg){//每一个进程维护一个PCB，entry是进程的入口地址，即函数的
	Area kstack = {.start = pcb->stack, .end = pcb->stack + STACK_SIZE};//[pcb->stack, pcb->stack+STACK_SIZE] 内核栈
	//Area kstack = {.start = &pcb->cp, .end = &pcb->cp + STACK_SIZE};
	pcb->cp = kcontext(kstack, entry, arg);
	return; 
}

void context_uload(PCB *pcb, void (*entry)(void *)){
	Area kstack = {.start = pcb->stack, .end = pcb->stack + STACK_SIZE};
	pcb->cp = ucontext(NULL, kstack, entry);
	pcb->cp->GPRx = heap.end;
	return;
}



void init_proc() {
	context_kload(&pcb[0], hello_fun, "one");//刚刚加载完的,还未开始运行进程,在进程的栈上人工创建一个上下文结构,
	context_kload(&pcb[1], hello_fun, "two");//内核线程：操作系统内部函数？？？？

	switch_boot_pcb();//初始化current指针

	yield();//收到EVENT_YIELD事件后, 调用schedule()并返回新的上下文

	Log("Initializing processes...");
	naive_uload(NULL,"/bin/nterm");//jump to entry....// load program here  bird nslider nterm menu
}

Context* schedule(Context *prev) {
	// save the context pointer
	current->cp = prev;	
	// always select pcb[0] as the new process
	//current = &pcb[0];
	current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
	// then return the new context
	return current->cp;
}


