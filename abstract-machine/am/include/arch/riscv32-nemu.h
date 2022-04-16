#ifndef ARCH_H__
#define ARCH_H__


//这些内容构成了完整的上下文信息, 异常处理过程可以根据上下文来诊断并进行处理, 同时, 将来恢复上下文的时候也需要这些信息.

//触发异常时的PC和处理器状态,异常号,地址空间.
struct Context {
  // TODO: fix the order of these members to match trap.S
  uintptr_t gpr[32], mcause, mstatus, mepc;
  //这只是变量名，与cpu.mcause等寄存器没有关联，换成a,b,c也是一样的
  //只要结构体内的成员顺序与trap.S中的栈中放置元素的顺序一样（小端模式，结构体低字节放在低地址）
  void *pdir;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10]  //a0
#define GPR3 gpr[11]  //a1
#define GPR4 gpr[12]  //a2
#define GPRx gpr[10]  //a0

#endif
