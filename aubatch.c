#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "dispatcher.h"
#include "utilities.h"

#define PLCY 		0
#define NUM_JOBS 	1
#define PRIORITIES	2
#define MIN_CPU_TM	3
#define MAX_CPU_TM	4

void print_help();
int get_case(char *input, int len);

/*
 * args :
 * <policy>
 * <num_of_jobs>
 * <priority_levels>
 * <min_CPU_time>
 * <max_CPU_time>
 */
int main(int argc, char **argv){

	//return error if not enough args
	if(argc < 6){
		fprintf(stderr, "\nnot enough arguements. run as:\n");
		fprintf(stderr, "./aubatch <policy> <num_jobs> <priority_levels> <min_cpu_time> <max_cpu_time>\n\n");
		return 1;
	}

	//args
	char *policy;
	char *num_jobs;
	char *priority_lvs;
	char *min_cpu_time;
	char *max_cpu_time;


	//create array of lengths of the args
	int i;
	int lens[5]={0};
	int len;
	for(i=0; i<5; i++){
		lens[i]=strlen(argv[i+1]);
	}

	//allocate memory for the arguements
	policy		 =malloc(sizeof(char)*lens[0]+1);
	num_jobs	 =malloc(sizeof(char)*lens[1]+1);
	priority_lvs =malloc(sizeof(char)*lens[2]+1);
	min_cpu_time =malloc(sizeof(char)*lens[3]+1);
	max_cpu_time =malloc(sizeof(char)*lens[4]+1);

	//copy args over
	strcpy(policy		, argv[1]);
	strcpy(num_jobs		, argv[2]);
	strcpy(priority_lvs	, argv[3]);
	strcpy(min_cpu_time	, argv[4]);
	strcpy(max_cpu_time	, argv[5]);

	//create threads
	pthread_t dispatcher;

	long arg=2;
	int res=pthread_create(&dispatcher, NULL, *dispatch, (void*) arg);
	//dispatch(2);

	//wait for threads to finish
	pthread_join(dispatcher, NULL);

	//main loop
	int result;
	int input_len=50;
	char input[input_len];
	printf("\nType \'help\' to show list of commands\n\n");
	while(1){
		printf("aubatch> ");
		//prompt for input
		
		//read input
		//scanf("%s\n", input);
		fgets(input, 30, stdin);

		result=get_case(input, input_len);

		switch (result){
			case 0:
				print_help();
				break;
			case 1:
				printf("list\n");
				break;
			case 2:
				printf("fcfs\n");
				break;
			case 3:
				printf("sjf\n");
				break;
			case 4:
				printf("priorty\n");
				break;
			case 5:
				printf("test\n");
				break;
			case 6:
				printf("\nexiting aubatch ...\n\n");
				return 0;
			default:
				break;
		}
	}

	
	
	//free args
	free(policy);
	free(num_jobs);
	free(priority_lvs);
	free(min_cpu_time);
	free(max_cpu_time);
	
	return 0;
}

void print_help(){

	printf("run <job> <time> <pri>: submit a job named <job>\n");
	printf("\t\texec time is <time>,\n");
	printf("\t\tpriority is <pri>\n");

	printf("list: display the job status\n");
	printf("fcfs: change the scheduling policy to FCFS\n");
	printf("sjf: change the scheduling policy to SJF\n");
	printf("priority: change the scheduling policy to priority\n");
	printf("test <benchmark> <policy> <num_of_jobs> <priority_levels>\n");
	printf("\t\tmin_cpu_time> <max_cpu_time>\n");
	printf("quit: exit aubatch\n\n");
	
	return;
}
int get_case(char *input, int len){

	char *command[7];
	command[0]="help\n";
	command[1]="list\n";
	command[2]="fcfs\n";
	command[3]="sjf\n";
	command[4]="priority\n";
	command[5]="test\n";
	command[6]="quit\n";

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
	//TODO make a more graceful exit routine
	else if (strcmp(input, "quit\n") == 0){
		return 6;
	}
	else{
		return -1;
	}
	
}


