#include <isa.h>

void dev_raise_intr() {
	cpu.INTR = true;//将INTR引脚设置为高电平
}
