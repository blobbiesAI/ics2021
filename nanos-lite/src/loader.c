#include <proc.h>
#include <elf.h>
#include <fs.h>
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
	//load file and return entry addr		
	//ramdisk_read((uint8_t*)0x83000000, 0x0, 0x050c0);
	//ramdisk_read((uint8_t*)0x830060c0, 0x0050c0, 0x00844);
	//memset((uint8_t*)(0x830060c0+0x00844), 0, 0x00880-0x00844);
	//return 0x83004bb0;
    int fd;
	fd = fs_open(filename, 0, 0);

	Elf_Ehdr elf_head;
	fs_read(fd, &elf_head, sizeof(Elf_Ehdr));//读取elf头表

	assert(*(uint32_t *)elf_head.e_ident == 0x464c457f);//检查魔数


	Elf_Phdr program_heads[elf_head.e_phnum];//程序头表有很多条目
	fs_lseek(fd, elf_head.e_phoff, SEEK_SET);
	fs_read(fd, program_heads, sizeof(program_heads));//读取程序头表

	size_t i;
	for(i = 0; i < elf_head.e_phnum; i++){
		Elf_Phdr cur = program_heads[i];
		if(cur.p_type == PT_LOAD){//load
			fs_lseek(fd, cur.p_offset, SEEK_SET);
			fs_read(fd, (void*)cur.p_vaddr, cur.p_filesz);
			memset((void*)(cur.p_vaddr + cur.p_filesz), 0, cur.p_memsz - cur.p_filesz);
			                                               //my bug:sizeof(cur.p_memsz - cur.p_filesz)
		}
	}
	return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);//返回程序入口地址
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();//强制转化为函数指针, 通过函数调用转移到用户进程的代码
}

