#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
/*
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  }
  return 0;
  */

  	printf("%s %s %s\n", argv[0], argv[1], argv[2]);
	return 0;

}
