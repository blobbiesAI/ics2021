#include <cpu/cpu.h>

void sdb_mainloop();

void engine_start() {
#ifdef CONFIG_TARGET_AM
  cpu_exec(-1);//arguments is the instructions nums execuable and is unsigned int, -1 is a big num when we treat it as uint64_t
#else
  /* Receive commands from user. */
  sdb_mainloop();
#endif
}
