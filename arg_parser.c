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
	
	//set i to 5 to ignore 'test ' in the input string
	char *i;
	i=input+5;
	char *token;

	
	
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
	int input_len=strlen(input);
	int argc=0;

	//set newline character to null and decrement strlen
	input[input_len-1]=0;
	input_len--;
	
	//set i to 5 to ignore 'run ' in the input string
	char *i;
	i=input+4;
	char *token;

	
	
	while ((token = strtok_r(i, " ", &i))){
		if (argc == 0){
			strcpy(r_args->job_name, token);
		}
		else if (argc == 1){
			r_args->time=atoi(token);
		}
		else if (argc == 2){
			r_args->priority=atoi(token);
		}
		argc+=1;
	}

	
	//set defaults if not enough args
	if (argc < 3){
		r_args->priority=DEF_PRIORITY;
	}
	if (argc < 2){
		r_args->time=3;
	}
	return argc;
}

