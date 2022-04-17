#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <NDL.h>
/*
int main(){
	printf("Hello. %d\n", sizeof(struct timeval));
    struct timeval time;
	uint32_t i = 5;
    while(1){
	    gettimeofday(&time, NULL);
		while(time.tv_sec<5){gettimeofday(&time, NULL);}
		printf("get time at %d second\n", i);
		i += 5;
    }
    return 0;
}
*/


int main(){
	NDL_Init(0);
    size_t i = 0;
	while(1){
		uint32_t t = NDL_GetTicks();
		while(t<5){
			t = NDL_GetTicks();
		}
		printf("gettimeofday %d\n", i);
		i += 10;
	}

	NDL_Quit();
}
