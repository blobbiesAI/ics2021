#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static char* opts[] = {"+", "-", "*"};
static char spaces[] = "    ";

static void gen_space(){
	snprintf(buf+strlen(buf), rand()%4, "%s", spaces);
	return;
}

static void gen_dec_num(){	
	gen_space();
	sprintf(buf+strlen(buf), "%u", (uint32_t)(rand()%9+1)); 
	gen_space();
	return;
}

static void gen_opt(){
	gen_space();
	sprintf(buf+strlen(buf), "%s", opts[rand()%3]);
	gen_space();
	return;
}

static void gen_rand_expr() {
	 if(strlen(buf)>=65535){return;}
	 switch(rand()%4){  
		case 0:                 //increase the gen_num probiblity
		case 1:	gen_dec_num();//di gui jiezhi tiaojian
				break;
		case 2: sprintf(buf+strlen(buf), "%s", "("); 
				gen_rand_expr();
				sprintf(buf+strlen(buf), "%s", ")");
				break;
		default:
				gen_rand_expr();
				gen_opt();
				gen_rand_expr();
				break;
	}
	return;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	buf[0] = '\0';//init
    gen_rand_expr();
	if( strlen(buf)>=65535 ){
		printf("buffer overflow!");
		continue;
	}

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
