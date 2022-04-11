#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lb) f(lbu) f(lh) f(lhu) f(lw) f(sb) f(sh) f(sw) f(addi) f(ori) f(andi) f(sltiu) f(slli) f(srli) f(srai) f(xori) f(auipc) f(jal) f(jalr) f(add) f(sub) f(mul) f(div) f(rem) f(remu) f(or) f(sll) f(mulh) f(sltu) f(slt) f(srl) f(sra) f(divu) f(xor) f(and) f(beq) f(bne) f(blt) f(bltu) f(bge) f(bgeu) f(inv) f(nemu_trap)

def_all_EXEC_ID();
