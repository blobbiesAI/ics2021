#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
	//def_rtl --->  rtl_li
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  rtl_addi(s, dest, src1, 0);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  TODO();
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  TODO();
}

static inline def_rtl(sexti, sword_t dest, const word_t src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
	switch(width){
		case 4: dest = (sword_t)(int32_t)src1; return;
		case 2: dest = (sword_t)(int16_t)src1; return;
		case 1: dest = (sword_t)( int8_t)src1; return;
		IFDEF(CONFIG_ISA64, case 8: dest = (sword_t)(int64_t)src1; return);
		IFDEF(CONFIG_RT_CHECK, default: assert(0));
	}
}


static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
	switch(width){
		case 4: *dest = (sword_t)(int32_t)(*src1); return;
		case 2: *dest = (sword_t)(int16_t)(*src1); return;
		case 1: *dest = (sword_t)( int8_t)(*src1); return;
		IFDEF(CONFIG_ISA64, case 8: *dest = (sword_t)(int64_t)(*src1); return);
		IFDEF(CONFIG_RT_CHECK, default: assert(0));
	}
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  TODO();
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  TODO();
}
#endif
