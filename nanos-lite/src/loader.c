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

typedef uintptr_t PTE;

static void* pgmalloc(size_t pg){
	void* p = new_page(pg);
	p -= pg*PGSIZE;
	memset(p, 0, pg*PGSIZE);
	return p;
}


//static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
	/*1.	load file and return entry addr		
		ramdisk_read((uint8_t*)0x83000000, 0x0, 0x050c0);
		ramdisk_read((uint8_t*)0x830060c0, 0x0050c0, 0x00844);
		memset((uint8_t*)(0x830060c0+0x00844), 0, 0x00880-0x00844);
		return 0x83004bb0;
	*/

	/*2.	pa3.load from elf file
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
	*/


	/*3.	pa4. add map of user process form virtual memory to physical memory  
	//TODO() find the bugs
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

			size_t page_n = ((cur.p_vaddr + cur.p_memsz - 1) >> 12) - (cur.p_vaddr >> 12) + 1;//每一load段多少页
			PTE* new_page = (PTE*)pgmalloc(page_n);//物理页

			printf("Loaded Segment from [%x to %x)\n", cur.p_vaddr, cur.p_vaddr + cur.p_memsz);	

			for(size_t j = 0; j<page_n; j++){
				map(&pcb->as, (void*)((cur.p_vaddr & ~0xfff) + j*PGSIZE), (void*)new_page + j*PGSIZE, 0);//构造虚拟页与物理页之间的映射
			}
		
			fs_read(fd, new_page + (cur.p_vaddr & 0xfff), cur.p_filesz);//将每一load段的内容复制到物理页
			memset((void*)(new_page + (cur.p_vaddr & 0xfff) + cur.p_filesz), 0, cur.p_memsz - cur.p_filesz);
			                                               //my bug:sizeof(cur.p_memsz - cur.p_filesz)
		}
	}
	fs_close(fd);
	return elf_head.e_entry;

}
*/


//github  Hilbert-Johnson/ics2021
static uintptr_t loader(PCB *pcb, const char *filename) {
	if (filename == NULL)
		panic("No filename given!");

	int fd = fs_open(filename, 0, 0);
	Elf_Ehdr ehdr;
	fs_read(fd, &ehdr, sizeof(Elf_Ehdr));

	// test
	assert(ehdr.e_ident[0] == 0x7f);
	assert(ehdr.e_ident[1] == 0x45);
	assert(ehdr.e_ident[2] == 0x4c);
	assert(ehdr.e_ident[3] == 0x46);

	Elf_Phdr phdr[ehdr.e_phnum];
	fs_lseek(fd, ehdr.e_ehsize, SEEK_SET);
	fs_read(fd, phdr, sizeof(Elf_Phdr)*ehdr.e_phnum);

	for (int i = 0; i < ehdr.e_phnum; i++){
		if (phdr[i].p_type == PT_LOAD) {
			uintptr_t vaddr_begin = (uintptr_t)phdr[i].p_vaddr & 0xfffff000;
			uintptr_t vaddr_end = ((uintptr_t)phdr[i].p_vaddr + phdr[i].p_memsz - 1) & 0xfffff000;
			int page_num = ((vaddr_end - vaddr_begin) >> 12) + 1;
			void *p = pgmalloc(page_num);
			for (int i = 0; i < page_num; i++){
				map(&pcb->as, (void *)(vaddr_begin + i * PGSIZE), p + i * PGSIZE, 0);
			}
			fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
			fs_read(fd, p + (phdr[i].p_vaddr & 0xfff), phdr[i].p_filesz);				
				/*
				memset((void *)phdr[i].p_vaddr, 0, phdr[i].p_memsz);
				fs_lseek(fd,phdr[i].p_offset,SEEK_SET);
				fs_read(fd,(void *)phdr[i].p_vaddr, phdr[i].p_filesz);
				*/
		}
	}
	fs_close(fd);
	return ehdr.e_entry;
}
void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);//返回程序入口地址
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();//强制转化为函数指针, 通过函数调用转移到用户进程的代码
}

uintptr_t proc_uload(PCB *pcb, const char *filename){
	uintptr_t entry = loader(pcb, filename);//返回程序入口地址
	return entry;
}
