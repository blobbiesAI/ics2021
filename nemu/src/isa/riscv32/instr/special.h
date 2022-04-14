
def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

/*
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

def_EHelper(mret){
	s->dnpc = cpu.mepc;
}


def_EHelper(ecall){
	s->dnpc = isa_raise_intr(1, s->pc);
}*/
