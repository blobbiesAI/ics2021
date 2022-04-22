.globl  _start
_start:
  move s0, zero
  move sp, a0   ###?????? 将CPRx中的值给栈指针
  jal call_main
