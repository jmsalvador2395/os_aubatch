#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

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

#define FL_TEST		0x01
#define FL_TERM		0x80
#define FL_SCHED	0x01
#define FL_RESCHED	0x02

//function declarations
void set_policy(int id, int *policy, pthread_mutex_t *policy_lock);
void print_stats(double count, double turnaround, double cpu, double wait);
int rand_int(int lower_bound, int upper_bound);
//void run_test(char *input);
void run_test(char *bench_name, char *policy, int num_jobs, int pri, int min_cpu, int max_cpu);

//declare mutex locks
pthread_mutex_t policy_lock;
pthread_mutex_t jq_lock;
pthread_mutex_t jp_lock;

//declare condition variables
pthread_cond_t sched_cv;
pthread_cond_t disp_cv;
pthread_cond_t test_cv;

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

// initialize average counters
double jobc=0;
double total_turnaround=0;
double total_cpu=0;
double total_wait=0;



/*
 * pops the job at the top of the queue and executes
 * stats are collected when finished
 */
void *dispatch_thread(){
	double wait=0;
	double cpu=0;
	double turnaround=0;
	struct timeval t1, t2;
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

		//calc wait time
		gettimeofday(&t1, NULL);
		wait=ms_time_diff(&jp->arrival_time, &t1);

		//signal scheduler and release mutex
		pthread_cond_signal(&sched_cv);
		pthread_mutex_unlock(&jq_lock);

		//*****this bottom half is outside the critical section*****

		//dispatch and then delete job from memory
		dispatch(jp->exec_time);

		//calc cpu time and turnaround time
		gettimeofday(&t2, NULL);
		cpu=ms_time_diff(&t1, &t2);
		turnaround=cpu+wait;

		//increment counters
		total_wait+=wait;
		total_cpu+=cpu;
		total_turnaround+=turnaround;
		jobc++;


		pthread_mutex_lock(&jp_lock);
		free(jp);
		jp=NULL;
		pthread_mutex_unlock(&jp_lock);

		if (get_qsize() == 0){
			pthread_cond_signal(&test_cv);
		}

		//check for termination request while executing job
		if (disp_flags & FL_TERM){
			pthread_cond_signal(&sched_cv);
			pthread_mutex_unlock(&jq_lock);
			break;
		}
	}
}

int main(int argc, char **argv){

	//initialize random number generator
	srand((unsigned) time(NULL));

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

	printf("\nWelcome to John's batch job scheduler Version 1.0");

	//create and execute threads
	pthread_t dispatcher, scheduler;
	int dispatch_res=pthread_create(&dispatcher, NULL, *dispatch_thread, NULL);

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
		int counted_args;
		int i;
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
					tm=asctime(localtime(&jp->arrival_date));
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
				set_policy(0, &pol_id, &policy_lock);
				pthread_mutex_lock(&jq_lock);
				reschedule_jobs(pol_id);
				printf("\n\nScheduling policy switched to %s. The %d jobs in the queue have been rescheduled\n\n", policies[0], get_qsize());
				pthread_mutex_unlock(&jq_lock);
				break;
			//sjf
			case 3:
				set_policy(1, &pol_id, &policy_lock);
				pthread_mutex_lock(&jq_lock);
				reschedule_jobs(pol_id);
				printf("\n\nScheduling policy switched to %s. The %d jobs in the queue have been rescheduled\n\n", policies[1], get_qsize());
				pthread_mutex_unlock(&jq_lock);
				break;
			//priority
			case 4:
				set_policy(2, &pol_id, &policy_lock);
				pthread_mutex_lock(&jq_lock);
				reschedule_jobs(pol_id);
				printf("\n\nScheduling policy switched to %s. The %d jobs in the queue have been rescheduled\n\n", policies[2], get_qsize());
				pthread_mutex_unlock(&jq_lock);
				break;
			//test
			case 5:
				counted_args=parse_test_args(&t_args, input);
				if (test_args_ok(counted_args, t_args)){
					run_test(t_args.bench_name, t_args.policy, t_args.num_jobs, t_args.priority, t_args.min_cpu_time, t_args.max_cpu_time);
				}
				break;
			//exit
			case 6:
				printf("\n**Waiting for threads to exit\n");
				//set termination flags for each thread
				sched_flags=FL_TERM;
				disp_flags=FL_TERM;

				//send termination signal to scheduler
				pthread_cond_signal(&disp_cv);

				//wait for threads to finish
				pthread_join(dispatcher, NULL);

				print_stats(jobc, total_turnaround, total_cpu, total_wait);

				printf("**exiting aubatch ...\n\n");
				
				//free the memory in the job queue
				free_jobq();
				return 0;
			//run a job
			case 7:

				counted_args=parse_run_args(&r_args, input);
				//run job if args are okay
				if (counted_args > 0){
					pthread_mutex_lock(&jq_lock);

					//wait for job queue to have space
					if (get_qsize() == JOBQ_MAX_SIZE)
						printf("Waiting for space in job queue ...\n");
					while (get_qsize() == JOBQ_MAX_SIZE){
						pthread_cond_wait(&sched_cv, &jq_lock);
					}

					int placement=createq_job(r_args.job_name, r_args.time, r_args.priority, pol_id);

					printf("\nTotal number of jobs in the queue: %d\n", get_qsize());
					int jp_time=0;
					if (jp != NULL) jp_time=jp->exec_time;
					printf("Expected waiting time: %d seconds\n", wait_time(placement)+jp_time);
					printf("Scheduling policy: %s\n\n", policies[pol_id]);

					pthread_cond_signal(&disp_cv);
					pthread_mutex_unlock(&jq_lock);
				}
				//print help menu if args not entered properly
				else{
					printf("\n** Not enough args **\n");
					print_help();
				}

				break;
			case 8:
				for (i=0; i < 3; i++){
					printf("\nbenchmarking %s ...\n", policies[i]);
					run_test("bench", policies[i], 10, 5, 10, 20);
				}
				break;
			default:
				break;
		}
	}

	return 0;
}
void set_policy(int id, int *policy, pthread_mutex_t *policy_lock){
	pthread_mutex_lock(policy_lock);
	
	*policy=id;

	pthread_mutex_unlock(policy_lock);

}

void print_stats(double count, double turnaround, double cpu, double wait){

	printf("===================================================\n");
	printf("Total number of jobs submitted: %d\n", (int) count);
	printf("Average turnaround time:\t%.2f seconds\n", turnaround/count/1000);
	printf("Average CPU time:\t\t%.2f seconds\n", cpu/count/1000);
	printf("Average waiting time:\t\t%.2f seconds\n", wait/count/1000);
	printf("Throughput:\t\t\t%2f jobs/second\n", count/turnaround*1000);
	printf("\n===================================================\n\n");


}

int rand_int(int lower_bound, int upper_bound){
	int num=rand()%(upper_bound+1-lower_bound);
	num+=lower_bound;
	return num;
}

//void run_test(char *input){
void run_test(char *bench_name, char *policy, int num_jobs, int pri, int min_cpu, int max_cpu){

	int temp_policy=pol_id;
	double temp_jobc=jobc;
	double temp_turnaround=total_turnaround;
	double temp_wait=total_wait;
	double temp_cpu=total_cpu;
	jobc=total_turnaround=total_wait=total_cpu=0;


	int test_pol_id=get_policy_id(policy);
	set_policy(test_pol_id, &pol_id, &policy_lock);

	int i;
	//loop for number of jobs
	for (i=0; i < num_jobs; i++){

		pthread_mutex_lock(&jq_lock);

		//wait for job queue to have space
		if (get_qsize() == JOBQ_MAX_SIZE)
			printf("Waiting for space in job queue ...\n");
		while (get_qsize() == JOBQ_MAX_SIZE){
			pthread_cond_wait(&sched_cv, &jq_lock);
		}

		int time=rand_int(min_cpu, max_cpu);
		int rnd_pri=rand_int(0, pri);
		int placement=createq_job("test_job", time, rnd_pri, test_pol_id);
			
		pthread_cond_signal(&disp_cv);
		pthread_mutex_unlock(&jq_lock);

		sleep(1);
	}

	//wait for jobs to terminate
	while (get_qsize() != 0){
		pthread_cond_wait(&test_cv, &jq_lock);
	}
	
	print_stats(jobc, total_turnaround, total_cpu, total_wait);
	jobc+=temp_jobc;
	total_turnaround+=temp_turnaround;
	total_wait+=temp_wait;
	total_cpu+=temp_cpu;
	pthread_mutex_unlock(&jq_lock);
	pol_id=temp_policy;
}



