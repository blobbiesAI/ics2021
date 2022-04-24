#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
	word_t vaddr31_22 = vaddr >> 22;
	word_t vaddr21_12 = (vaddr >> 12) & 0x3ff;
	//word_t pg_offset  = vaddr & 0xfff;

	word_t base_entry_addr = (cpu.satp << 12) + 4 * vaddr31_22;
	word_t secd_entry_addr = ((paddr_read(base_entry_addr, 4) >> 10) << 12) + 4 * vaddr21_12;
		
	word_t ret             = paddr_read(secd_entry_addr, 4);
	
	panic("temp breakpoint.");
	return ret; 
}
