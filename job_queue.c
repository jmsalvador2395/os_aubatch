#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "dispatcher.h"
#include "utilities.h"
#include "job_queue.h"



//job queue
struct job *jobq[QUEUE_SIZE];

/*
 * creates job based on given parameters and pushes it into the job queue
 */
int create_job(char *job_name, int exec_time, int priority){

	struct job *new_job;
	
	//allocate memory for job
	new_job=malloc(sizeof(struct job));

	//assign parameters
	strcpy(new_job->job_name, job_name);
	new_job->exec_time=exec_time;
	new_job->priority=priority;
	new_job->arrival_time=time(NULL);
	new_job->status=0;

	//push job into queue
	push_job(new_job);
	return 0;
}

int push_job(struct job *new_job){

	jobq[0]=new_job;
}


/*
 * TODO
 */
struct job* pop_job(){

	return jobq[0];
}

/*
 * TODO
 */
void reschedule_jobs(char *alg){

	printf("rescheduling %s\n", alg);
	return;
}
