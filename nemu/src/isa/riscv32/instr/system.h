/*
def_EHelper(csrrs){
	switch(id_src2->imm){
		case 0x341:
			dsrc2 = &(cpu.mepc);
			break;
		case 0x342:
			dsrc2 = &(cpu.mcause);
			break;
		case 0x300:
			dsrc2 = &(cpu.mstatus);
			break;
		case 0x305:
			dsrc2 = &(cpu.mtvec);
			break;
	}
	rtl_addi(s, ddest, dsrc2, 0);
	rtl_or(s, dsrc2, dsrc1, dsrc2);
}

def_EHelper(csrrw){
	switch(id_src2->imm){
		case 0x341:
			dsrc2 = &(cpu.mepc);
			break;
		case 0x342:
			dsrc2 = &(cpu.mcause);
			break;
		case 0x300:
			dsrc2 = &(cpu.mstatus);
			break;
		case 0x305:
			dsrc2 = &(cpu.mtvec);
			break;
	}
	rtl_addi(s, ddest, dsrc2, 0);
	rtl_addi(s, dsrc2, dsrc1, 0);
}

def_EHelper(mret){
	rtl_j(s, cpu.mepc);
}

def_EHelper(ecall){
	isa_raise_intr(1,cpu.pc);
	rtl_j(s, cpu.mtvec);
}

*/

def_EHelper(csrrw){
	switch(id_src2->imm){
		case 0x300: //if(ddest != &GPRx){*ddest = cpu.mstatus;}
					cpu.mstatus = *dsrc1;
					break;
		case 0x305: //if(ddest != &gpr(0)){*ddest = cpu.mtvec;}
					cpu.mtvec = *dsrc1;
					break;
		case 0x341: //if(ddest != &gpr(0)){*ddest = cpu.mepc;}
					cpu.mepc = *dsrc1;
					break;
		case 0x342: //if(ddest != &gpr(0)){*ddest = cpu.mcause;}
					cpu.mcause = *dsrc1;
					break;
		default:
			panic("Unvalid CSR register.\n");
	}
}


def_EHelper(csrrs){
	switch(id_src2->imm){
		case 0x300: *ddest = cpu.mstatus;
					//if(dsrc1 != &gpr(0)){cpu.mstatus |= *dsrc1;}
					break;
		case 0x305: *ddest = cpu.mtvec;
					//if(dsrc1 != &gpr(0)){cpu.mtvec |= *dsrc1;}
					break;
		case 0x341: *ddest = cpu.mepc;
					//if(dsrc1 != &gpr(0)){cpu.mepc |= *dsrc1;}
					break;
		case 0x342: *ddest = cpu.mcause;
					//if(dsrc1 != &gpr(0)){cpu.mcause |= *dsrc1;}
					break;
		default:
			panic("Unvalid CSR register.\n");
	} 
}

def_EHelper(mret){//返回自陷指令的下一条指令
	s->dnpc = cpu.mepc+4;
}


def_EHelper(ecall){
	s->dnpc = isa_raise_intr(1, s->pc);
}
