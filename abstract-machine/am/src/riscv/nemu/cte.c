#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>


void __am_get_cur_as(Context *c);
void __am_switch(Context *c);


static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {//在trap.S中，__am_irq_handle函数往上是上下文信息保存，往下是上下文信息恢复

  __am_get_cur_as(c);//将satp寄存器值保存在上下文中
  //printf("before switch context:%p\n", c->pdir);
   if (user_handler) {
    Event ev = {0};
     switch (c->mcause) {//isa_raise_intr(); def_EHelper(ecall) 128
		//通过将context结构体中异常号识别出自陷异常, 并打包成编号为EVENT_YIELD的自陷事件.
		case 128:	if(c->GPR1==-1){ev.event = EVENT_YIELD;}
					else{ev.event = EVENT_SYSCALL;}
					c->mepc += 4;  //异常的下一条指令
					break;
		case 0x80000007:
					ev.event = EVENT_IRQ_TIMER;
					break;
		default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
	//printf("%08x %08x %08x\n", c->mcause, c->mstatus, c->mepc);
  }
  __am_switch(c);//从上下文中将地址空间指针保存在satp寄存器（此时进程已切换）
  //printf("after switch context:%p\n",c->pdir);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));//全局符号,设置中断异常处理入口地址
  // register event handler
  user_handler = handler;

  return true;
}


#define MPIE 0x80;
Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
	Context *p = (Context*)(kstack.end - sizeof(Context));
	p->mepc = (uintptr_t)entry;/////????
	p->gpr[10] = (uintptr_t)arg;//a0
	p->mstatus = MPIE;  
	p->pdir = NULL;
	//如果是刚加载的进程，entry则是入口地址，否则是异常点pc值（或下一条指令），从__am_asm_trap返回时，最后会执行mret指令，将cpu的pc指向mepc寄存器中保存的地址,则会开始运行当前PCB对应的进程
	return p;
}

void yield() {
  asm volatile("li a7, -1; ecall");//给寄存器a7赋值-1.  a7就是gpr[17]，即GPR1，这一点至关重要。
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
