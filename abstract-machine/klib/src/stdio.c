#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


static int numtostr(char* out, int len, int num){
	if(num<0){out[len++] = '-'; num = -num;}
	int start = len;

	while(num>0){
		out[len++] = num%10 + '0';
		num /= 10;
	}

	char temp; 
	int end = len-1; 
	while(start<end){
		temp = out[start];
		out[start] = out[end];
		out[end] = temp;
		start++; end--;
	}
	return len;
}



int printf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	size_t i = 0;
	size_t len = 0;

	char* s; int d;
	while(fmt[i]){
		if(fmt[i]!='%'){
			putch(fmt[i]);
			i++;
			len++;
			continue;
		}
	
		i++;//skip %
		switch(fmt[i]){
			case 's':
				s = va_arg(ap, char*);
				size_t n = strlen(s);
				for(size_t j = 0; j<n; j++){
					putch(s[j]);
				}
				len += n;		
				break;
			case 'd':
				d = va_arg(ap, int);
				char out[20];
				size_t m = numtostr(out, 0, d);
				for(size_t k = 0; k<m; k++){
					putch(out[k]);
				}
				len += m;
				break;
		}
		
		i++;
	}
	
	va_end(ap);
	return len;

}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}



int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	size_t i = 0, len = 0;


	char* s; int d;
	while(fmt[i]){
		if(fmt[i]!='%'){
			out[len] = fmt[i];
			i++;
			len++;
			continue;
		}
	
		i++;//skip %
		switch(fmt[i]){
			case 's':
				s = va_arg(ap, char*);
				size_t n = strlen(s);
				for(size_t j = 0; j<n; j++){
					out[len++] = s[j];
				}		
				break;
			case 'd':
				d = va_arg(ap, int);
				len = numtostr(out, len, d);
				break;
		}
		
		i++;
	}
	
	va_end(ap);
	out[len] = '\0';
	return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
