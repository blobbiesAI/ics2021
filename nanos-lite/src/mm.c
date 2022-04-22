#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
	//pf初始是heap.start(看init_mm函数)，而之前讲义里都是从heap.end考虑，容易出错
	pf += nr_page * 4096;
    return pf;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  return NULL;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
