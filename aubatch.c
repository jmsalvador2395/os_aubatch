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


pthread_mutex_t policy_lock;
pthread_mutex_t mutex;

int job_queue;
char *policy;

void print_help();
int get_case(char *input, int len);

void *dispatch_thread(void *args){
	//pthread_mutex_lock(&mutex);
	printf("acquired lock\n");
	//dispatch(args);
	int i;
	for (i=0; i < 100; i++)
		printf("dispatcher %d\n", i);
	//pthread_mutex_unlock(&mutex);
	printf("released lock\n");
}
void scheduler_thread(void *args){

}

/*
 * args :
 * <policy>
 * <num_of_jobs>
 * <priority_levels>
 * <min_CPU_time>
 * <max_CPU_time>
 */
int main(int argc, char **argv){

	if (pthread_mutex_init(&mutex, NULL) != 0){
		fprintf(stderr, "\nmutex init failed\n");
		return 1;
	}

	if (pthread_mutex_init(&policy_lock, NULL) != 0){
		fprintf(stderr, "\npolicy_lock init failed\n");
		return 1;
	}

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
	int res=pthread_create(&dispatcher, NULL, *dispatch_thread, (void*) arg);
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
			//print help
			case 0:
				print_help();
				break;
			//list
			case 1:
				printf("list\n");
				break;
			//fcfs
			case 2:
				pthread_mutex_lock(&mutex);
				policy="fcfs";
				pthread_mutex_unlock(&mutex);
				printf("\n**policy changed to fcfs\n\n");
				break;
			//sjf
			case 3:
				pthread_mutex_lock(&mutex);
				policy="sjf";
				pthread_mutex_unlock(&mutex);
				printf("\n**policy changed to sjf\n\n");
				break;
			//priority
			case 4:
				pthread_mutex_lock(&mutex);
				policy="priority";
				pthread_mutex_unlock(&mutex);
				printf("\n**policy changed to priority\n\n");
				break;
			//test
			case 5:
				printf("test\n");
				break;
			//exit
			case 6:
				printf("\n**exiting aubatch ...\n\n");
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


