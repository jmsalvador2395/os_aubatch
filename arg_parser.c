#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arg_parser.h"



/*
 * parse test args. returns the number of args parsed
 */
int parse_test_args(struct test_args *t_args, char *input){
	int input_len=strlen(input);
	int argc=0;

	//set newline character to null and decrement strlen
	input[input_len-1]=0;
	input_len--;
	
	short int space_encountered=0;
	//set i to 5 to ignore 'test '
	char *i;
	i=input+5;
	char *token;

	
	//set default cpu times
	t_args->min_cpu_time=MIN_CPU_TM;
	t_args->max_cpu_time=MAX_CPU_TM;
	
	while ((token = strtok_r(i, " ", &i))){
		if (argc == 0){
			strcpy(t_args->bench_name, token);
		}
		else if (argc == 1){
			strcpy(t_args->policy, token);
		}
		else if (argc == 2){
			t_args->num_jobs=atoi(token);
		}
		else if (argc == 3){
			t_args->priority=atoi(token);
		}
		else if (argc == 4){
			t_args->min_cpu_time=atoi(token);
		}
		else if (argc == 5){
			t_args->max_cpu_time=atoi(token);
		}
		argc+=1;
	}
	return argc;
}

int parse_run_args(struct run_args *r_args, char *input){
	printf("parse_run_args\n");
}

