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

	PTE secd_entry_addr = ((paddr_read(base_entry_addr, 4) >> 10) << 12) + 4 * vaddr21_12;

	PTE ret             = paddr_read(secd_entry_addr, 4);
	//printf("vpn:%08x %08lx\n", vaddr & ~0xfff, (ret>>10) <<12);	
	//panic("temp breakpoint.");
	ret = ((ret >> 10) << 12) | (vaddr & 0xfff);
	return ret; 
}



/*github  Hilbert-Johnson/ics2021
union Satp{
	struct{
		uint32_t root_addr : 22;
		uint32_t ASID : 9;
		uint32_t MODE : 1;
	};
	uint32_t val;
};

union PageTableEntry{
	struct {
		uintptr_t V : 1;
		uintptr_t R : 1;
		uintptr_t W : 1;
		uintptr_t X : 1;
		uintptr_t U : 1;
		uintptr_t G : 1;
		uintptr_t A : 1;
		uintptr_t D : 1;
		uintptr_t RSW : 2;
		uintptr_t PPN : 22;
	};
	uintptr_t val;
};

union VirtualAddress{
	struct {
		uintptr_t offset : 12;
		uintptr_t page : 10;
		uintptr_t dir : 10;
	};
	uintptr_t val;
};

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
	union Satp satp;
	satp.val = cpu.satp;
	union VirtualAddress VA;
	VA.val = vaddr;
	union PageTableEntry directory, page;
	directory.val = paddr_read((satp.root_addr << 12) + 4*VA.dir, 4);
	if (directory.V == 0)
		assert(0);
	page.val = paddr_read((directory.PPN << 12) + 4 * VA.page, 4);
	if (page.V == 0)
		assert(0);
	paddr_t paddr = (page.PPN << 12) + VA.offset;
	return paddr | MEM_RET_OK;
}
*/
