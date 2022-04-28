#include <proc.h>
#include <fs.h>
#include <errno.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);
uintptr_t proc_uload(PCB *pcb, const char *filename);

//只用于新的进程启动，即人工创建的上下文信息存在PCB内核栈中，运行时的上下文存在用户栈
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

//static int free_pcb = 0;


void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
	//if(j%10000==0){
		Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    //}
	j ++;
    yield();
  }
}


void context_kload(PCB *pcb, void (*entry)(void *), void *arg){//每一个进程维护一个PCB，entry是进程的入口地址，即函数的
	Area kstack = {.start = pcb->stack, .end = pcb->stack + STACK_SIZE};//[pcb->stack, pcb->stack+STACK_SIZE] 内核栈
	//Area kstack = {.start = &pcb->cp, .end = &pcb->cp + STACK_SIZE};
	pcb->cp = kcontext(kstack, entry, arg);
	printf("kernal process entry:%p, kprocess context:%p\n", entry, pcb->cp); 
	return; 
}

void context_uload(PCB *pcb, const char* filename, char *const argv[], char *const envp[]){//构造一个进程的初始的环境，包括PCB和用户栈，PCB中内核栈中保存初始上下文信息，但运行过程中的上下文信息是保存在用户栈中的
	
	//
	protect(&pcb->as);

	Area kstack = {.start = pcb->stack, .end = pcb->stack + STACK_SIZE};//每一个pcb里有一个内核栈（即一个数组),栈底放着Context，栈顶放着cp指针.
	uintptr_t entry = proc_uload(pcb, filename);//加载用户程序,返回进程入口地址
	pcb->cp = ucontext(&pcb->as, kstack, (void*)entry);//上下文结构指针  ??????
	printf("user process entry:%p, uprocess context:%p, heap_start:%p, heap end:%p\n", entry, pcb->cp, heap.start, heap.end);


	/********从heap.start开始，每个进程往上获得32KB大小用户栈空间，新进程继续往上获得32KB......**********/
	size_t nr_page = 8;
	char*p = (char*)new_page(nr_page);//8*4kb,  mm.c
	pcb->cp->GPRx = (uintptr_t)p;
	printf("new page:%08x\n", p);

	//vp->pp
	for(size_t i = 1; i<=nr_page; i++){
		map(&pcb->as, pcb->as.area.end - i*PGSIZE, p - i*PGSIZE , 0);
	}

	/*argv和envp参数数组的成员数*/	
	size_t argc = 0, envc = 0;
	for(; argv[argc]!=NULL; argc++);
	for(; envp[envc]!=NULL; envc++);
	printf("argv: %d envp: %d\n", argc, envc);

	/*****参数字符串放到栈中*****/
	uintptr_t argv_addr[argc], envp_addr[envc];
	for(int i = 0; i < argc; i++){
		p -= (strlen(argv[i]) + 1);
		strcpy(p, argv[i]);
		argv_addr[i] = (uintptr_t)p;
	}

	for(int j = 0; j < envc; j++){
		p -= (strlen(envp[j]) + 1);
		strcpy(p, envp[j]);
		envp_addr[j] = (uintptr_t)p;
	}
	
	p -= 8;
	memset(p, 0, 8);//留白
	/*****参数字符串地址放到栈中*****/
	uint32_t* q = (uint32_t*)p;
	for(int j = envc-1; j>=0; j--){   //不可size_t j = envc-1
		q--;
		*q = envp_addr[j];	
	}
	q--;
	*q = 0;//argv和envp之间4字节NULL
	//
	for(int i = argc-1; i>=0; i--){
		q--;
		*q = argv_addr[i];	
	}
	q--;
	*q = argc;//argc值

	pcb->cp->GPRx = (uintptr_t)q;//(uintptr_t)pcb->as.area.end - (pcb->cp->GPRx - (uintptr_t)q);//虚拟地址栈顶
	printf("after user process init, sp pointer:%p\n", pcb->cp->GPRx);
	return;
/*
	*********用户程序无参数**********
	pcb->cp->GPRx = (uintptr_t)heap.end;
	//把堆区用作用户栈,堆end作为栈顶（0x88000000）
	//刚加载完用户程序后，若无参数，则将heap.end作为栈顶指针.
	//这里将栈顶指针的值赋给GPRx，在用户程序的crt0/_start中在将GPRx的值给sp指针
	**********************************


	*********用户程序有参数**********

	void* pseudo_sp = heap.end;

	*argv和envp参数数组的成员数*	
	size_t argc = 0, envc = 0;
	for(; argv[argc]!=NULL; argc++);
	for(; envp[envc]!=NULL; envc++);
	printf("argv: %d envp: %d\n", argc, envc);

	*****参数字符串放到栈中*****
	char* p = (char*)pseudo_sp;
	uintptr_t argv_addr[argc], envp_addr[envc];
	for(int i = 0; i < argc; i++){
		p -= (strlen(argv[i]) + 1);
		strcpy(p, argv[i]);
		argv_addr[i] = (uintptr_t)p;
	}

	for(int j = 0; j < envc; j++){
		p -= (strlen(envp[j]) + 1);
		strcpy(p, envp[j]);
		envp_addr[j] = (uintptr_t)p;
	}
	
	p -= 8;
	memset(p, 0, 8);//留白
	*****参数字符串地址放到栈中*****
	uint32_t* q = (uint32_t*)p;
	for(int j = envc-1; j>=0; j--){   //不可size_t j = envc-1
		q--;
		*q = envp_addr[j];	
	}
	q--;
	*q = 0;//argv和envp之间4字节NULL
	//
	for(int i = argc-1; i>=0; i--){
		q--;
		*q = argv_addr[i];	
	}
	q--;
	*q = argc;//argc值

	pcb->cp->GPRx = (uintptr_t)q;
	printf("after user process init, sp pointer:%p\n", pcb->cp->GPRx);
	return;
*/
}

/*
static size_t ceil_4_bytes(size_t size){
	  if (size & 0x3)
		      return (size & (~0x3)) + 0x4;
			    return size;
}
*/

enum{KERNEL_THREAD, PROC_NTERM, PROC_HELLO, PROC_DUMMY};

char *uproc[] = {
	[KERNEL_THREAD] = NULL,
	[PROC_NTERM] = "/bin/nterm",
	[PROC_HELLO] = "/bin/hello",
	[PROC_DUMMY] = "/bin/dummy",
	//user process
};


char* argv[MAX_NR_PROC][8] = {
	[PROC_NTERM] = {"/bin/nterm", NULL},
	[PROC_HELLO] = {"/bin/hello", NULL},
	[PROC_DUMMY] = {"/bin/dummy", NULL},
	//user process argv[]
};


char* envp[MAX_NR_PROC][8] = {
	[PROC_NTERM] = {NULL},
	[PROC_HELLO] = {NULL},
	[PROC_DUMMY] = {NULL},
	//user process envp[]
};




void init_proc() {
	context_kload(&pcb[KERNEL_THREAD], hello_fun, "one");//刚刚加载完的,还未开始运行进程,在进程的栈上人工创建一个上下文结构,
	//context_kload(&pcb[1], hello_fun, "two");//内核线程：操作系统内部函数？？？？

	context_uload(&pcb[1], uproc[1], argv[1], envp[1]);
	context_uload(&pcb[2], uproc[2], argv[2], envp[2]);
	context_uload(&pcb[3], uproc[3], argv[3], envp[3]);

	//--------------program break-----------------//
	extern char _end;
	printf("program break(_end):%p\n", &_end);
	//assert(0);
	//-------------------end----------------------//


	switch_boot_pcb();//初始化current指针

	//yield();//收到EVENT_YIELD事件后, 调用schedule()并返回新的上下文

	Log("Initializing processes...");
	//naive_uload(&pcb[0],"/bin/dummy");//jump to entry....// load program here  bird nslider nterm menu
}

Context* schedule(Context *prev) {//进程调度
	// save the context pointer
	current->cp = prev;	
	// always select pcb[0] as the new process
	//current = &pcb[0];
	//current = &pcb[2];
	//free_pcb++;
	current = (current == &pcb[2] ? &pcb[1] : &pcb[2]);
	// then return the new context
	return current->cp;
}

int pexecve(const char* filename, char *const argv[], char *const envp[]){
	if (fs_open(filename, 0, 0) < 0){// 文件不存在
		return -1;
	}
	printf("Loading from %s ...\n", filename);

	//A的执行流中创建用户进程B
	context_uload(&pcb[PROC_DUMMY], filename, argv, envp);
	//要运行exec-test,只需要current = &pcb[3];和context_uload(&pcb[3], filename, argv, envp);
	//通过new_page不断构建32KB大小用户栈,那内核栈

	//结束A的执行流并切换到B进程
	switch_boot_pcb();
	yield();
	return 0;
}
