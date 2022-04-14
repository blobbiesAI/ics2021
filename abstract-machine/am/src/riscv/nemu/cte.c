#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {//在trap.S中，__am_irq_handle函数往上是上下文信息保存，往下是上下文信息恢复
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) { 
		//通过将context结构体中异常号识别出自陷异常, 并打包成编号为EVENT_YIELD的自陷事件.
		case 1:  ev.event = EVENT_YIELD; break;
		default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
	//printf("%08x %08x %08x\n", c->mcause, c->mstatus, c->mepc);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));//全局符号
  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
