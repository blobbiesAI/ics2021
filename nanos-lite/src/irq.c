#include <common.h>
//#include "syscall.h"

void do_syscall(Context*);
Context* schedule(Context *);

static Context* do_event(Event e, Context* c) {
  switch (e.event)  {
	case EVENT_YIELD:   Log("yield ok!"); 
						c = schedule(c);
						//c->GPR1 = (uintptr_t)schedule(c);
						//printf("%08x\n", c->GPR1);
						break;
	case EVENT_SYSCALL: 
						//Log("syscall ok!");
						do_syscall(c);	
						break;
	default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
