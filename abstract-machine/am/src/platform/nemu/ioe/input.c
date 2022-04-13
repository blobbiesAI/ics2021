#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keyword = inw(KBD_ADDR);
  uint32_t stat = (keyword & KEYDOWN_MASK)>>15;
  
  kbd->keydown = (stat==1);
  kbd->keycode = keyword & (~KEYDOWN_MASK);
}
