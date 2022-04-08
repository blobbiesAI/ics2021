#include "../local-include/reg.h"
#include <cpu/ifetch.h>
#include <isa-all-instr.h>

def_all_THelper();

static uint32_t get_instr(Decode *s) {
  return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static def_DopHelper(i) {//immediate 
  op->imm = val;
}

static def_DopHelper(r) {//register
  bool is_write = flag;
  static word_t zero_null = 0;//hei dong bianliang
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);//input into 0 register
  //give the address of one register to Decode*s
}

static def_DHelper(I) {//decode_I
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
  decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(U) {// put data into Decode*s 
  decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);//give src immediate to s
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);//give destination register to s
}



static def_DHelper(S) {//decode_S
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}


static def_DHelper(J){
	sword_t simm = s->isa.instr.j.imm9_0 | (s->isa.instr.j.imm10 << 10);
	simm |= (s->isa.instr.j.imm18_11 << 11);
	simm |= (s->isa.instr.j.imm19 << 19); 
	if(s->isa.instr.j.imm19==1){//sign extend
		simm |= (0xfff << 20);
	}
	decode_op_i(s, id_src1, simm << 1, false);
    decode_op_r(s, id_dest, s->isa.instr.j.rd, true);
}

def_THelper(load) {//table_load
   def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
  //if lw, return table_lw
  //else return EXEC_ID_inv
  return EXEC_ID_inv;
}

def_THelper(store) {//table_store
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
  return EXEC_ID_inv;
}

def_THelper(imme){
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
  return EXEC_ID_inv;
}

def_THelper(main) {//table_main
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I     , load);//pattern, id, tab
  //-->decode_I(operation data)-->return table_load(operation code idx)
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S     , store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U     , lui);
   //-->decode_I(operation data)-->return table_lui()
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I     , imme);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U     , auipc);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J     , jal);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I     , jalr);

  def_INSTR_TAB  ("??????? ????? ????? ??? ????? 11010 11",         nemu_trap);
 //else 
  return table_inv(s);
  // EXEC_ID_inv
};

int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);//FETCH, use union to store instruction, extract opcode or opdata later
  int idx = table_main(s);//DECODE
  return idx;
}
