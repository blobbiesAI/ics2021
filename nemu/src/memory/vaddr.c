#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
	int systatus = isa_mmu_check(addr, len, 0);
	if(systatus == MMU_TRANSLATE){
		paddr_t ret = isa_mmu_translate(addr, len, 0);
		ret |= (addr & 0xfff);
		if(ret!=addr){
			panic("ret do not equal to addr");
		}
		addr = ret;
	}
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
	int systatus = isa_mmu_check(addr, len, 0);
	if(systatus == MMU_TRANSLATE){
		paddr_t ret = isa_mmu_translate(addr, len, 0);
		ret |= (addr & 0xfff);
		assert(ret==addr);
		addr = ret;
	}

  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
	int systatus = isa_mmu_check(addr, len, 0);
	if(systatus == MMU_TRANSLATE){
		paddr_t ret = isa_mmu_translate(addr, len, 0);
		ret |= (addr & 0xfff);
		assert(ret==addr);
		addr = ret;
	}

  paddr_write(addr, len, data);
}
