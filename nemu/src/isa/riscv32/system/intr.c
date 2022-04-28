#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mepc = epc;
  cpu.mcause = NO;
 
  cpu.mstatus.m.MPIE = cpu.mstatus.m.MIE;
  cpu.mstatus.m.MIE  = 0;
  return cpu.mtvec;
}


#define IRQ_TIMER 0x80000007
word_t isa_query_intr() {
	if (cpu.mstatus.m.MIE==1 && cpu.INTR==true) {
		cpu.INTR = false;
		//引脚的高低由外部设备决定的，cpu是否处理中断，由引脚和cpu是否是关中断的状态有关（mstatus）
		return IRQ_TIMER;//时钟中断号
	 }
	return INTR_EMPTY;
}
