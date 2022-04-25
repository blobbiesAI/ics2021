#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

typedef uintptr_t PTE;

inline int isa_mmu_check(vaddr_t vaddr, int len, int type){
	return (cpu.satp >> 31)==1 ? MMU_TRANSLATE : MMU_DIRECT;
}


paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
	PTE vaddr31_22 = vaddr >> 22;
	PTE vaddr21_12 = (vaddr >> 12) & 0x3ff;
	//word_t pg_offset  = vaddr & 0xfff;
	PTE base_entry_addr = (cpu.satp << 12) + 4 * vaddr31_22;
	//printf("cpu.satp:%08lx\n", base_entry_addr);

	PTE secd_entry_addr = ((paddr_read(base_entry_addr, 4) >> 10) << 12) + 4 * vaddr21_12;
	

	//printf("secd_entry_addr:%08lx\n", secd_entry_addr);


	PTE ret             = paddr_read(secd_entry_addr, 4);
	//printf("vpn:%08x %08lx\n", vaddr & ~0xfff, (ret>>10) <<12);	
	//panic("temp breakpoint.");
	return (ret >> 10) << 12 ;//ppn 
}
