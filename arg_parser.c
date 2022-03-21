#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arg_parser.h"

char *sched_policies[3] = {"fcfs", "sjf", "priority"};


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

int test_args_ok(int argc, struct test_args t_args){
	int errflag=1;
	printf("\n");
	if (argc < 6){
		printf("*Not enough args\n");
		errflag=0;
	}
	if (t_args.num_jobs==0){
		printf("*Number of jobs needs to be greater than 0\n");
		errflag=0;
	}
	if (t_args.priority < 1){
		printf("*Priority levels needs to be greater than 0\n");
		errflag=0;
	}
	if (!valid_scheduling(t_args.policy)){
		printf("*Invalid scheduling policy\n");
		errflag=0;
	}
	if (t_args.max_cpu_time < 1 || t_args.min_cpu_time < 1){
		printf("*cpu times need to be at least 1\n");
		errflag=0;
	}
	if (t_args.max_cpu_time-t_args.min_cpu_time < 0){
		printf("*max cpu time needs to be higher than min cpu time\n");
		errflag=0;
	}
	printf("\n");
	return errflag;
}

int get_case(char *input, int len){
	
	//check if help is entered
	if (strcmp(input, "help\n") == 0){
		return 0;
	}
	//check if list is entered
	else if (strcmp(input, "list\n") == 0){
		return 1;
	}
	//check if fcfs is entered
	else if (strcmp(input, "fcfs\n") == 0){
		return 2;
	}
	//check if sjf is entered
	else if (strcmp(input, "sjf\n") == 0){
		return 3;
	}
	//check if priority is entered
	else if (strcmp(input, "priority\n") == 0){
		return 4;
	}
	//check if test is entered
	else if (strstr(input, "test")){
		return 5;
	}
	//check if exit reoutine is called
	else if (strcmp(input, "quit\n") == 0){
		return 6;
	}
	//check if job is submitted
	else if (strstr(input, "run")){
		return 7;
	}
	else if (strcmp(input, "evaluate_all\n") == 0){
		return 8;
	}
	else{
		return -1;
	}
	
}

void print_help(){

	printf("\trun <job> <time> <pri>: submit a job named <job>\n");
	printf("\t\t\t\texec time is <time>,\n");
	printf("\t\t\t\tpriority is <pri>\n");

	printf("\tlist:\tdisplay the job status\n");
	printf("\tfcfs:\tchange the scheduling policy to FCFS\n");
	printf("\tsjf:\tchange the scheduling policy to SJF\n");
	printf("\tpriority: change the scheduling policy to priority\n");
	printf("\ttest <benchmark> <policy> <num_of_jobs> <priority_levels>\n");
	printf("\t\t<min_cpu_time> <max_cpu_time>\n");
	printf("\tevaluate_all: test all policies with preset parameters\n\n");
	printf("\tquit:\texit aubatch\n\n");
	
	return;
}


int valid_scheduling(char *policy){
	int i=0;
	for (i=0; i<3; i++){
		if (strcmp(policy, sched_policies[i]) == 0){
			return 1;
		}
	}
	return 0;
}

int get_policy_id(char *c){
	int i=0;
	for (i=0; i<3; i++){
		if (strcmp(c, sched_policies[i]) == 0){
			return i;
		}
	}
	return 0;
}
