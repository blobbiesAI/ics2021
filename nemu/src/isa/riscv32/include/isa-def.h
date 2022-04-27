#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>
#include <isa.h>
typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  rtlreg_t mepc;
  rtlreg_t mcause;
  rtlreg_t mtvec;
  rtlreg_t mstatus;
  rtlreg_t satp;
  vaddr_t pc;
} riscv32_CPU_state;

// decode
typedef struct {
  union {
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm31_12  :20;
    } u;
	struct {
	  uint32_t opcode1_0 : 2;
	  uint32_t opcode6_2 : 5;
	  uint32_t rd		 : 5;
	  uint32_t imm18_11  : 8;
	  uint32_t imm10     : 1;
	  uint32_t imm9_0    :10;
	  int32_t  imm19     : 1;
	} j;
	struct {
	  uint32_t opcode1_0 : 2;
	  uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
	  uint32_t funct3    : 3;
	  uint32_t rs1       : 5;
	  uint32_t rs2       : 5;
	  uint32_t funct7    : 7;
	} r;
	struct {
	  uint32_t opcode1_0 : 2;
	  uint32_t opcode6_2 : 5;
	  uint32_t imm10     : 1;
	  uint32_t imm3_0    : 4;
	  uint32_t funct3    : 3;
	  uint32_t rs1       : 5;
	  uint32_t rs2       : 5;
	  uint32_t imm9_4    : 6;
	  int32_t  imm11      : 1;
	} b;
    uint32_t val;
  } instr;
} riscv32_ISADecodeInfo;

//#define isa_mmu_check(vaddr, len, type) (MMU_TRANSLATE)

#endif
