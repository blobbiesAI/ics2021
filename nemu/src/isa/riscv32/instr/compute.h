def_EHelper(lui) {//exec_lui
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi){
	rtl_addi(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(xori){
	rtl_xori(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(auipc){
	rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(jal){
	rtl_mv(s, ddest, &s->snpc);
	rtl_addi(s, &s->dnpc, &s->pc, id_src1->simm);
}

def_EHelper(jalr){
	rtl_mv(s, ddest, &s->snpc);
	rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm & ~1);
}

def_EHelper(add){
	rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub){
	rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul){
	rtl_mulu_lo(s, ddest, dsrc1, dsrc2);	
}

def_EHelper(or){
	rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll){
	rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltiu){
	rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->simm);	
}

def_EHelper(beq){
	if(*dsrc1 == *dsrc2){
		rtl_addi(s, &s->dnpc, &s->pc, id_dest->simm);
	}
}

def_EHelper(bne){
	if(*dsrc1 != *dsrc2){
		rtl_addi(s, &s->dnpc, &s->pc, id_dest->simm);
	}
}

def_EHelper(bge){
	if(*dsrc1 >= *dsrc2){
		rtl_addi(s, &s->dnpc, &s->pc, id_dest->simm);
	}
}

def_EHelper(slli){
	rtl_slli(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(srli){
	rtl_srli(s, ddest, dsrc1, id_src2->simm);
}
