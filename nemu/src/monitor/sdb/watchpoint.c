#include <stdlib.h>
#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char str[32];
  uint32_t value;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};

static WP *dummyhead = NULL, *dummyfree = NULL;

void init_wp_pool() {
	dummyhead = (WP*)malloc(sizeof(WP)); 
	dummyhead->NO = -1; 
	dummyhead->next = NULL;

	dummyfree = (WP*)malloc(sizeof(WP));
	dummyfree->NO = -1;
	dummyfree->next = NULL;

  int i;
  for (i =  0; i < NR_WP; i ++) { 
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  dummyhead->next = NULL;
  dummyfree->next = wp_pool;
}


/* TODO: Implement the functionality of watchpoint */

int new_wp(char* args){
	if(dummyfree->next==NULL){Assert(0,"No watchpoint available.");}
	//get from free list
	WP* cur = dummyfree->next;
	dummyfree->next = dummyfree->next->next;
	
	//insert into used list
	strcpy(cur->str, args);
	cur->next = dummyhead->next;
	dummyhead->next = cur;

	return cur->NO;
}

int free_wp(char* args){
	if(args==NULL){
		dummyhead->next = NULL;
		dummyfree->next = wp_pool;
		return -1;
	}

	//get from head list
	WP *p = dummyhead->next, *before = dummyhead;
	for( ;p!=NULL; p = p->next, before = before->next){
		if(p->NO == atoi(args)){
			before->next = p->next;

			//insert into free list
			p->next = dummyfree->next;
			dummyfree->next = p;
			break;
		}
	}	
	return p->NO;
}


int check_using_list(int* num, int* old, int* new){
	word_t wp_val;
	bool success = true;
	for(WP* p = dummyhead->next; p!=NULL; p = p->next){
		wp_val = expr(p->str, &success);
		if(wp_val!=p->value){
			*num = p->NO;
			*old = p->value;
			*new = wp_val;

			p->value = wp_val;
			return -1;	
		}
	}
	return 0;
}


void soft_wp_display(){
	printf("Num     Type           What\n");
	if(dummyhead->next==NULL){
		printf("No watchpoints.\n");
		return;
	}
	for(WP* p = dummyhead->next; p!=NULL; p = p->next){
		printf("%d     sw watchpoint     %s\n", p->NO, p->str);
	}
	return;
}

