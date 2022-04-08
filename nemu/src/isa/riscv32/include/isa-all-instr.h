#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lbu) f(lh) f(lhu) f(lw) f(sh) f(sw) f(addi) f(sltiu) f(slli) f(srli) f(xori) f(auipc) f(jal) f(jalr) f(add) f(sub) f(mul) f(or) f(sll) f(beq) f(bne) f(bge) f(inv) f(nemu_trap)

def_all_EXEC_ID();
