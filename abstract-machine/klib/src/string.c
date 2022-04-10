#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t i;
	for(i = 0; s[i]!='\0'; i++){}
	return i;
}

char *strcpy(char *dst, const char *src) {
	size_t n = strlen(src)+1;
	size_t i;
	for(i = 0; i<n; i++){
		dst[i] = src[i];
	}
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	size_t i;
	for(i=0; i<n && src[i]!='\0'; i++){
		dst[i] = src[i];
	}
	for(; i<n; i++){
		dst[i] = '\0';
	}
	return dst;
}

char *strcat(char *dst, const char *src) {
	size_t srcn = strlen(src)+1;
	size_t dstn = strlen(dst);
	size_t i;
	for(i = 0; i < srcn; i++){
		dst[dstn+i] = src[i];
	}
	return dst;
}

int strcmp(const char *s1, const char *s2) {
	size_t s1_len = strlen(s1);
	size_t s2_len = strlen(s2);
	size_t i;
	int diff = 0;
	for(i = 0; i<s1_len && i<s2_len; i++){
		diff = s1[i]-s2[i];
		if(diff){return diff;}
	}
	if(s1_len==s2_len){return 0;}
	else if(s1_len < s2_len){return -s2[i];}
	else{return s1[i];}
}

int strncmp(const char *s1, const char *s2, size_t n) {//????
	size_t i;
	int diff = 0;
	for(i = 0; i<n; i++){
		diff = s1[i]-s2[i];
		if(diff){return diff;}
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	size_t i;
	int8_t* p = (int8_t*)s;
	for(i = 0; i<n; i++){
		p[i] = c;
	}
	return (void*)s;
}

void *memmove(void *dst, const void *src, size_t n) {
	size_t i;
	int8_t*p = (int8_t*)dst;
	int8_t*q = (int8_t*)src;
	if(*p < *q){
		for(i = n-1; i>=0; i--){
			p[i] = q[i];
		}
	}
	else{
		for(i = 0; i<n; i++){
			p[i] = q[i];
		}
	}
	return (void*)dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	size_t i;
	int8_t*p = (int8_t*)out;
	int8_t*q = (int8_t*)in;
	for(i = 0; i<n; i++){
		p[i] = q[i];
	}
	return (void*)out;
}

int memcmp(const void *s1, const void *s2, size_t n) {//???
	size_t i;
	int diff = 0;
	uint8_t* p = (uint8_t*)s1;
	uint8_t* q = (uint8_t*)s2; 
	for(i = 0; i<n; i++){
		diff = p[i]-q[i];
		if(diff){return diff;}
	}
	return 0;
}

#endif
