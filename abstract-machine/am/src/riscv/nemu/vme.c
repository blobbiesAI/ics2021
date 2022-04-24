#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};   //abstract-machine/am/include/am.h
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE          //abstract-machine/am/src/platform/nemu/include/nemu.h
};                         

#define USER_SPACE RANGE(0x40000000, 0x80000000)

//mode(31): 0-bare, 1-Sv32; 
//ASID(30-22); 
//PPN(21-0);
static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);//一级页表所在的物理页; ptr是物理页的物理地址

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);//Sv32是二级页表，类似一个链表，给出第一级页表的地址就可以唯一确定
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {//上下文切换时，也需要切换映射关系,即页表
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {//添加一条页表条目（从一级页表到二级页表）
	PTE va31_22 = (PTE)va >> 22;
	PTE va21_12 = ((PTE)va >> 12) & 0x3ff;	

	//一级页表某一条目
	PTE *base_entry = (PTE*)as->ptr + va31_22; //指向某一条目的指针;PTE = uintptr_t
	
	if(*base_entry == 0){
		
		//二级页表物理地址	
		PTE secd_base_entry = (PTE)(pgalloc_usr(PGSIZE));
		printf("0x%08x\n", secd_base_entry);	
		//二级页表物理地址放在一级页表某一条目
		*base_entry = (secd_base_entry >> 12) << 10;
	}
	
	//二级页表某一条目
	PTE* secd_entry = (PTE*)(((*base_entry) >> 10) << 12) + va21_12; 
	
	//物理地址填到二级页表某一条目
	*secd_entry = ((PTE)pa >> 12) << 10  ;
	return;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
	Context *p = (Context*)(kstack.end - sizeof(Context));
	p->mepc = (uintptr_t)entry;
	return p;
}
