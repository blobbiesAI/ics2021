#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
word_t vaddr_read(vaddr_t , int );

enum {
  TK_NOTYPE = 256, 
  TK_EQ, TK_NEQ,
  TK_REG,
  TK_DEC, TK_HEX,
  TK_DEREF,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},           //sub
  {"\\*", '*'},         //mul
  {"\\/", '/'},          //div
  {"\\(", '('},           //left p
  {"\\)", ')'},           //right p
  {"0x[0-9a-f]+", TK_HEX},   //16 based num 
  {"[0-9]+", TK_DEC},     //10 based num
  {"\\${1,2}[0-9a-z]+", TK_REG},    //register 
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       //not equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
			case TK_DEC: tokens[nr_token].type = TK_DEC; 
			             strncpy(tokens[nr_token].str, substr_start, substr_len);
						 nr_token++;
						 break;  
			case TK_HEX: tokens[nr_token].type = TK_HEX;
						 strncpy(tokens[nr_token].str, substr_start, substr_len);
						 nr_token++;
						 break;  
			case TK_REG: tokens[nr_token].type = TK_REG;
						 strncpy(tokens[nr_token].str, substr_start, substr_len);
						 nr_token++;
						 break;  
			case TK_EQ:  tokens[nr_token].type = TK_EQ;
						 nr_token++;
						 break;
			case TK_NEQ: tokens[nr_token].type = TK_NEQ;
						 nr_token++;
						 break;

			case '+':   tokens[nr_token].type = '+';
						nr_token++;
						break;
			case '-':   tokens[nr_token].type = '-';
						nr_token++;
						break;
			case '*':   tokens[nr_token].type = '*';
						nr_token++;
						break;
			case '/':   tokens[nr_token].type = '/';
						nr_token++;
						break;
			case '(':   tokens[nr_token].type = '(';
						nr_token++;
						break;
			case ')':   tokens[nr_token].type = ')';
						nr_token++;
						break;
            default: 
					break;
				
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  
  if(nr_token==0){return false;}// if only space
  return true;
}


int check_parenthesis_matching(uint32_t p, uint32_t q){
	//0:yes;(1+2)          1:valid but no yes(1+2)+(1+2);     2:invalid (1+2))
	int stack[32]; int sp = -1;
	for(int i = p; i<=q; i++){
		if(tokens[i].type !='(' && tokens[i].type !=')'){continue;}
		else{
			if(sp==-1 || tokens[i].type=='('){stack[++sp] = i;}
			else{
				if(tokens[stack[sp]].type == '('){
					if(sp==0 && i==q && stack[sp]==p){return 0;}
					--sp;
				}
				else{return 2;}
			}
		}
	}

	return sp>-1 ? 2 : 1;
}


int find_main_opt(uint32_t p, uint32_t q){
	char priority[300]={0};//300 must > token type
	priority[TK_EQ] = 1; priority[TK_NEQ] = 1;
	priority['+'] = 2; priority['-'] = 2;
	priority['*'] = 3; priority['/'] = 3;
	priority[TK_DEREF] = 4;

	int flag = 0;
	int candidate_opt = -1;
	
	//compare priority 
	for(int i = p; i<=q; i++){
		if(flag==1){// flag = 1 indicate in the ()
			if(tokens[i].type == ')'){flag = 0;}
			continue;
		}
		else{
			if(tokens[i].type == '('){flag = 1;}
			if(priority[tokens[i].type]==0){continue;}// not operations
			else{
				if(candidate_opt==-1){candidate_opt = i;}//first operation
				else{
					candidate_opt = (priority[tokens[i].type]<=priority[tokens[candidate_opt].type] ? i : candidate_opt);
				}
			}
		}
	}
	//printf("%d\n", candidate_opt);
	//Assert(candidate_opt!=-1, "find no operation, go into wrong branch!");
	return candidate_opt;
}


uint32_t eval(int32_t p, int32_t q, bool * success){
	if(p > q){
		//*success = false; 
		return 0;
	}
	else if(p == q){
			switch(tokens[p].type){
				case TK_DEC: return (uint32_t)strtol(tokens[p].str, NULL, 10);
							 break;
				case TK_HEX: return (uint32_t)strtol(tokens[p].str, NULL, 16);
                             break;
				case TK_REG: return isa_reg_str2val(tokens[p].str+1, success);
							 break;
				default:
						*success = false;
						break;
			}
			return 0;
		}
	else{
		int flag = check_parenthesis_matching(p,q);
		printf("flag:%d\n", flag);
		if(flag==0){return eval(p+1, q-1, success);}
		else if(flag==2){*success=false; return 0;}
		else{
			int mopt_idx = find_main_opt(p, q);
			printf("mopt_idx:%d\n", mopt_idx);
			if(mopt_idx == -1){//do not have operation
                char temp[64]={'\0'};
				for(int i = p; i<=q; i++){// 1 2 3+1 =124
					sprintf(temp+strlen(temp), "%s", tokens[i].str);
				}
				return (uint32_t)strtol(temp, NULL, 10);
			}

			uint32_t l = eval(p, mopt_idx - 1, success);
			//printf("%u\n", l);
			uint32_t r = eval(mopt_idx + 1, q, success);
			//printf("%u\n", r );
			switch(tokens[mopt_idx].type){
				case TK_DEREF: return l + vaddr_read(r, 4);
				case TK_EQ: return l==r;
				case TK_NEQ: return l!=r;
				case '+': return l+r;
				case '-': return l-r;
				case '*': return l*r;
				case '/': return l/r;
				default:
					assert(0);
			}			
		}
	}
	return 0;
}




word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
	printf("nr_token:%d\n", nr_token);

	for(int i = 0; i<nr_token; i++){
		if(tokens[i].type=='*' && (i==0 || tokens[i-1].type=='+' 
			|| tokens[i-1].type=='-' || tokens[i-1].type=='/'
			|| tokens[i-1].type=='(' || tokens[i-1].type==TK_EQ
			|| tokens[i-1].type==TK_NEQ)){
			
			tokens[i].type=TK_DEREF;
			}
	}
  /* TODO: Insert codes to evaluate the expression. */
	word_t output = eval(0, nr_token-1, success);
    return output;
}
