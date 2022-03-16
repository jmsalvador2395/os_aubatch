#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "dispatcher.h"
#include "utilities.h"
#include "job_queue.h"

#define PLCY 		0
#define NUM_JOBS 	1
#define PRIORITIES	2
#define MIN_CPU_TM	3
#define MAX_CPU_TM	4



pthread_mutex_t policy_lock;
pthread_mutex_t mutex;

char *policy;

void print_help();
int get_case(char *input, int len);

void *dispatch_thread(void *args){
	//pthread_mutex_lock(&mutex);
	printf("acquired lock\n");
	//dispatch(args);
	int i;
	/*
	for (i=0; i < 100; i++)
		printf("dispatcher %d\n", i);
	*/
	//pthread_mutex_unlock(&mutex);
	printf("released lock\n");
}
void *scheduler_thread(void *args){
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

	/*
	create_job("jimmy", 0, 0);
	struct job *my_job;
	my_job=pop_job();
	*/



	if (pthread_mutex_init(&mutex, NULL) != 0){
		fprintf(stderr, "\nmutex init failed\n");
		return 1;
	}

	if (pthread_mutex_init(&policy_lock, NULL) != 0){
		fprintf(stderr, "\npolicy_lock init failed\n");
		return 1;
	}

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
				pthread_mutex_lock(&policy_lock);
				policy="fcfs";
				pthread_mutex_unlock(&policy_lock);
				reschedule_jobs(policy);
				printf("\n**policy changed to fcfs. job queue rescheduled\n\n");
				break;
			//sjf
			case 3:
				pthread_mutex_lock(&policy_lock);
				policy="sjf";
				pthread_mutex_unlock(&policy_lock);
				reschedule_jobs(policy);
				printf("\n**policy changed to sjf. job queue rescheduled\n\n");
				break;
			//priority
			case 4:
				pthread_mutex_lock(&policy_lock);
				policy="priority";
				pthread_mutex_unlock(&policy_lock);
				reschedule_jobs(policy);
				printf("\n**policy changed to priority. job queue rescheduled\n\n");
				break;
			//test
			case 5:
				printf("test\n");
				break;
			//exit
			case 6:
				printf("\n**exiting aubatch ...\n\n");
				return 0;
			case 7:
				printf("\n**run job ...\n\n");
				break;
			default:
				break;
		}
	}

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
	
	int cmd=0;
	char *commands[7]={"help\n", "list\n", "fcfs\n", "sjf\n", "priority\n", "test\n", "quit"};

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
	else if (strstr(input, "run")){
		return 7;
	}
	else{
		return -1;
	}
	
}


