#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "dispatcher.h"
#include "utilities.h"
#include "job_utils.h"

#define PLCY 		0
#define NUM_JOBS 	1
#define PRIORITIES	2
#define MIN_CPU_TM	3
#define MAX_CPU_TM	4

#define FL_TERM		0x80
#define FL_SCHED	0x01
#define FL_RESCHED	0x02



pthread_mutex_t policy_lock;
pthread_mutex_t jq_lock;

pthread_cond_t sched_cv;
pthread_cond_t disp_cv;

unsigned char sched_flags=0;
unsigned char disp_flags=0;

int terminate=0;
int test=0;

char *policy;

void print_help();
int get_case(char *input, int len);

/*
 * pops the job at the top of the queue and executes
 * states are collected when finished
 */
void *dispatch_thread(){
	while(1){
		pthread_mutex_lock(&jq_lock);

		//wait for new job or termination request
		while (test == 0 && !disp_flags){
			pthread_cond_wait(&disp_cv, &jq_lock);
		}
		
		//check for termination request
		if (disp_flags & FL_TERM){
			pthread_cond_signal(&sched_cv);
			pthread_mutex_unlock(&jq_lock);
			break;
		}

		//test--;
		//printf("dispatcher decremented\n");

		//signal scheduler and release mutex
		disp_flags=0x00;
		pthread_cond_signal(&sched_cv);
		pthread_mutex_unlock(&jq_lock);
	}
}

/*
 * puts the submitted jobs into the job queue
 */
void *scheduler_thread(){
	while(1){
		pthread_mutex_lock(&jq_lock);

		//wait for job queue to have space
		while (test == 10 && !sched_flags){
			pthread_cond_wait(&sched_cv, &jq_lock);
		}

		//check for termination request
		if (sched_flags & FL_TERM){
			pthread_cond_signal(&disp_cv);
			pthread_mutex_unlock(&jq_lock);
			break;
		}
		else if(sched_flags & FL_SCHED){
			printf("schedule called\n");
			createq_job("job1", 2, 1);
			print_jobq();
			sched_flags=0x00;
		}
		else if (sched_flags & FL_RESCHED){
			printf("reschedule called");
			sched_flags=0x00;
		}

		//test++;
		//printf("scheduler incremented\n");

		//signal dispatcher and release mutex
		pthread_cond_signal(&disp_cv);
		pthread_mutex_unlock(&jq_lock);
	}
}

int main(int argc, char **argv){

	//initialize mutexes
	if (pthread_mutex_init(&jq_lock, NULL) != 0){
		fprintf(stderr, "\nmutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&policy_lock, NULL) != 0){
		fprintf(stderr, "\npolicy_lock init failed\n");
		return 1;
	}

	//create and execute threads
	pthread_t dispatcher, scheduler;
	int dispatch_res=pthread_create(&dispatcher, NULL, *dispatch_thread, NULL);
	int scheduler_res=pthread_create(&scheduler, NULL, *scheduler_thread, NULL);

	//enter the main loop
	int result;
	int input_len=50;
	char input[input_len];
	printf("\nType \'help\' to show list of commands\n\n");
	while(1){
		printf("aubatch> ");
		//prompt for input
		
		//read input
		//scanf("%s\n", input);
		fgets(input, input_len, stdin);

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

				sched_flags=FL_RESCHED;

				pthread_cond_signal(&sched_cv);
				printf("\n**policy changed to fcfs. job queue rescheduled\n\n");
				break;
			//sjf
			case 3:
				pthread_mutex_lock(&policy_lock);
				policy="sjf";
				pthread_mutex_unlock(&policy_lock);

				sched_flags=FL_RESCHED;

				pthread_cond_signal(&sched_cv);
				printf("\n**policy changed to sjf. job queue rescheduled\n\n");
				break;
			//priority
			case 4:
				pthread_mutex_lock(&policy_lock);
				policy="priority";
				pthread_mutex_unlock(&policy_lock);

				sched_flags=FL_RESCHED;

				pthread_cond_signal(&sched_cv);
				printf("\n**policy changed to priority. job queue rescheduled\n\n");
				break;
			//test
			case 5:
				printf("test\n");
				break;
			//exit
			case 6:
				printf("\n**exiting aubatch ...\n\n");
				//wait for threads to finish
				sched_flags=FL_TERM;
				disp_flags=FL_TERM;

				pthread_cond_signal(&disp_cv);
				pthread_cond_signal(&sched_cv);

				pthread_join(dispatcher, NULL);
				pthread_join(scheduler, NULL);
				free_jobq();
				print_jobq();
				return 0;
			case 7:
				printf("\n**run job ...\n\n");
				sched_flags=FL_SCHED;

				pthread_cond_signal(&disp_cv);
				pthread_cond_signal(&sched_cv);
				break;
			default:
				break;
		}
	}

	return 0;
}

void print_help(){

	printf("\trun <job> <time> <pri>: submit a job named <job>\n");
	printf("\t\t\t\texec time is <time>,\n");
	printf("\t\t\t\tpriority is <pri>\n");

	printf("\tlist: display the job status\n");
	printf("\tfcfs: change the scheduling policy to FCFS\n");
	printf("\tsjf: change the scheduling policy to SJF\n");
	printf("\tpriority: change the scheduling policy to priority\n");
	printf("\ttest <benchmark> <policy> <num_of_jobs> <priority_levels>\n");
	printf("\t\t<min_cpu_time> <max_cpu_time>\n");
	printf("\tquit: exit aubatch\n\n");
	
	return;
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
	else{
		return -1;
	}
	
}


