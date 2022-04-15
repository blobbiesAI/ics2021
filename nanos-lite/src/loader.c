#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
	//load file and return entry addr	
	ramdisk_read((void*)0x83000000, 0x0, 0x04d4c);
	ramdisk_read((void*)0x83005000, 0x005000, 0x00870);
	memset((void*)(0x83005000+0x00870), 0, 0x008ac-0x00870);
	return 0x83000000;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

