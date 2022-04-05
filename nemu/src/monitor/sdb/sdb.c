#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "memory/vaddr.h"
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char* args){
	if(args==NULL){
		cpu_exec(1);
		return 0; 
	}
	uint64_t i_N = (uint64_t)atoll(args);
    cpu_exec(i_N);
    return 0;
}


static void print_register(){
	isa_reg_display();
	return;	
}

void soft_wp_display();
static void print_watchpoint(){
	soft_wp_display();
	return;
}

static int cmd_info(char* args){
	Assert(args, "Arguments missing");
     switch(args[0]){
		case 'r': print_register(); break;
		case 'w': print_watchpoint(); break;
		default:
			printf("Uknow argument!");
			break;
	}
	return 0;
}


static int cmd_x(char* args){
	Assert(args, "Arguments missing");
	char* count_memr = strtok(args, " ");
	char* start_addr = count_memr + strlen(count_memr) + 1;

	int N_four_bytes = atoi(count_memr);	
	vaddr_t begin_addr = strtol(start_addr, NULL, 16);	
	
	word_t memr_v;
	for(int idx = 0; idx<N_four_bytes;  idx++){
		memr_v = vaddr_read(begin_addr+4*idx, 4);
		printf("%08x: %08x %u\n", begin_addr+4*idx, memr_v, memr_v);
	}
	return 0;
}

word_t expr(char*, bool*);
static int cmd_p(char* args){
	Assert(args, "Arguments missing");
	bool success = true;
	word_t output = expr(args, &success);
	if(success==true){ printf("HEX:0x%08x DEC:%u\n", output, output); return 0;}
	else{ 
		printf("Bad expression, incredible answer!\n");		
	}
	return 0;
}


int new_wp(char* args);
static int cmd_watch(char* args){
	Assert(args, "Arguments missing");
	int wp_id;
	wp_id = new_wp(args);
	printf("Software watchpoint %d: %s\n", wp_id, args);
	return 0;	
}

int free_wp(char* args);
static int cmd_d(char* args){
	int wp_id;
	wp_id = free_wp(args);
	if(wp_id<0){
		printf("Delete all watchpoints.\n");
	}
	else{
		printf("Software watchpoint %d at %s deleted.\n", wp_id, args);
	}
	return 0;	
}


static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {//running raguments
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si", "Let the program step through N instructions and then pause execution. When N is not given, the default is 1", cmd_si},
  {"info", "Print register status (r) or Print Watchpoint Information (w)", cmd_info},
  {"x", "Print N 4 bytes memory value from start addr", cmd_x},
  {"p", "Expression evaluation", cmd_p},
  {"watch", "Set up a new watchpoint", cmd_watch},
  {"d", "Delete watchpoint N.", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }//q return -1, mainloop reutrn
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
