#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "dispatcher.h"
#include "utilities.h"
#include "job_utils.h"
#include "arg_parser.h"

#define PLCY 			0
#define NUM_JOBS 		1
#define PRIORITIES		2
#define MIN_CPU_TM		3
#define MAX_CPU_TM		4
#define NAME_LEN 		30
#define POL_NAME_LEN	8

#define FL_TERM		0x80
#define FL_SCHED	0x01
#define FL_RESCHED	0x02

//function declarations
void print_help();
int get_case(char *input, int len);
int test_args_ok(int argc, struct test_args t_args);
void set_policy(int id, int *policy, pthread_mutex_t *policy_lock);

//declare mutex locks
pthread_mutex_t policy_lock;
pthread_mutex_t jq_lock;
pthread_mutex_t jp_lock;

//declare condition variables
pthread_cond_t sched_cv;
pthread_cond_t disp_cv;

//declare flags
unsigned char sched_flags=0;
unsigned char disp_flags=0;

//policy variables
int pol_id=0;
char *policies[3] = {"fcfs", "sjf", "priority"};

//initialize arg structs
struct test_args t_args;
struct run_args r_args;

struct job *jp;

/*
 * pops the job at the top of the queue and executes
 * stats are collected when finished
 */
void *dispatch_thread(){
	while(1){
		//enter critical section to pop job out of queue
		pthread_mutex_lock(&jq_lock);

		//wait for new job or termination request
		while (get_qsize() == 0 && !disp_flags){
			pthread_cond_wait(&disp_cv, &jq_lock);
		}
		
		//check for termination request
		if (disp_flags & FL_TERM){
			pthread_cond_signal(&sched_cv);
			pthread_mutex_unlock(&jq_lock);
			break;
		}

		//access job queue
		pthread_mutex_lock(&jp_lock);
		jp=popq_job();
		pthread_mutex_unlock(&jp_lock);

		//signal scheduler and release mutex
		pthread_cond_signal(&sched_cv);
		pthread_mutex_unlock(&jq_lock);

		//*****this half is outside the critical section*****

		//dispatch and then delete job from memory
		dispatch(jp->exec_time);

		pthread_mutex_lock(&jp_lock);
		free(jp);
		jp=NULL;
		pthread_mutex_unlock(&jp_lock);

		//check for termination request while executing job
		if (disp_flags & FL_TERM){
			pthread_cond_signal(&sched_cv);
			pthread_mutex_unlock(&jq_lock);
			break;
		}
	}
}

/*
 * puts the submitted jobs into the job queue
 */
void *scheduler_thread(){
	while(1){
		pthread_mutex_lock(&jq_lock);

		//wait for job queue to have space
		while (get_qsize() == JOBQ_MAX_SIZE && !sched_flags){
			pthread_cond_wait(&sched_cv, &jq_lock);
		}

		//check for termination request
		if (sched_flags & FL_TERM){
			pthread_cond_signal(&disp_cv);
			pthread_mutex_unlock(&jq_lock);
			break;
		}
		//check for a new job
		else if(sched_flags & FL_SCHED){
			createq_job(r_args.job_name, r_args.time, r_args.priority, pol_id);
			sched_flags=0x00;
		}
		//check for rescheduling
		else if (sched_flags & FL_RESCHED){
			printf("reschedule called\n");
			sched_flags=0x00;
		}

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
	if (pthread_mutex_init(&jp_lock, NULL) != 0){
		fprintf(stderr, "\npolicy_lock init failed\n");
		return 1;
	}



	//create and execute threads
	pthread_t dispatcher, scheduler;
	int dispatch_res=pthread_create(&dispatcher, NULL, *dispatch_thread, NULL);
	int scheduler_res=pthread_create(&scheduler, NULL, *scheduler_thread, NULL);

	//enter the main loop
	int result;
	int counted_args;
	int input_len=50;
	char input[input_len];
	printf("\nType \'help\' to show list of commands\n\n");
	while(1){
		counted_args=0;

		printf("aubatch> ");
		
		//read input
		fgets(input, input_len, stdin);
		result=get_case(input, input_len);

		switch (result){
			//print help
			case 0:
				print_help();
				break;
			//list
			case 1:
				pthread_mutex_lock(&jp_lock);
				pthread_mutex_lock(&jq_lock);
				printf("\n===================================================\n");
				printf("Number of jobs in the queue: %d\n", get_qsize());
				printf("Scheduling Policy: %s\n", policies[pol_id]);
				printf("\nName\t\tCPU_Time\tPri\tArrival_time\t\t\tProgress\n");
				if (jp != NULL){
					char *tm;
					tm=asctime(localtime(&jp->arrival_time));
					tm[strlen(tm)-1]=0;
					printf("%s\t\t%d\t\t%d\t%s\t%s\n",
								jp->job_name,
								jp->exec_time,
								jp->priority,
								tm,
								"run");
				}

				print_jobq();

				pthread_mutex_unlock(&jp_lock);
				pthread_mutex_unlock(&jq_lock);
				printf("===================================================\n\n");
				break;
			//fcfs
			case 2:
				//set policy id variable
				set_policy(0, &pol_id, &policy_lock);

				//set scheduler flags
				sched_flags=FL_RESCHED;

				//signal the scheduler
				pthread_cond_signal(&sched_cv);
				printf("\n**policy changed to fcfs. job queue rescheduled\n\n");
				break;
			//sjf
			case 3:
				//set policy id variable
				set_policy(1, &pol_id, &policy_lock);

				//set scheduler flags
				sched_flags=FL_RESCHED;

				//signal the scheduler
				pthread_cond_signal(&sched_cv);
				printf("\n**policy changed to sjf. job queue rescheduled\n\n");
				break;
			//priority
			case 4:
				//set policy id variable
				set_policy(2, &pol_id, &policy_lock);

				//set scheduler flags
				sched_flags=FL_RESCHED;

				//signal the scheduler
				pthread_cond_signal(&sched_cv);
				printf("\n**policy changed to priority. job queue rescheduled\n\n");
				break;
			//test
			case 5:
				counted_args=parse_test_args(&t_args, input);
				if (test_args_ok(counted_args, t_args)){
					printf("test\n");
				}
				else{
					printf("\n** Not enough args **\n");
					print_help();
				}
				break;
			//exit
			case 6:
				printf("\n**Waiting for threads to exit\n");
				//set termination flags for each thread
				sched_flags=FL_TERM;
				disp_flags=FL_TERM;

				//send termination signal to scheduler
				pthread_cond_signal(&sched_cv);

				//wait for threads to finish
				pthread_join(dispatcher, NULL);
				pthread_join(scheduler, NULL);

				printf("**exiting aubatch ...\n\n");
				
				//free the memory in the job queue
				free_jobq();
				return 0;
			//run a job
			case 7:

				counted_args=parse_run_args(&r_args, input);
				//run job if args are okay
				if (counted_args > 0){
					sched_flags=FL_SCHED;
					pthread_cond_signal(&disp_cv);
				}
				//print help menu if args not entered properly
				else{
					printf("\n** Not enough args **\n");
					print_help();
				}

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

	printf("\tlist:\tdisplay the job status\n");
	printf("\tfcfs:\tchange the scheduling policy to FCFS\n");
	printf("\tsjf:\tchange the scheduling policy to SJF\n");
	printf("\tpriority: change the scheduling policy to priority\n");
	printf("\ttest <benchmark> <policy> <num_of_jobs> <priority_levels>\n");
	printf("\t\t<min_cpu_time> <max_cpu_time>\n");
	printf("\tquit:\texit aubatch\n\n");
	
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

int test_args_ok(int argc, struct test_args t_args){
	if (argc < 6){
		return 0;
	}
	return 1;
}

void set_policy(int id, int *policy, pthread_mutex_t *policy_lock){
	pthread_mutex_lock(policy_lock);
	
	*policy=id;
	printf("set policy to %d\n", *policy);

	pthread_mutex_unlock(policy_lock);

}
