#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "dispatcher.h"
#include "utilities.h"
#include "job_utils.h"


int job_list_size=0;
int qsize=0;
int qhead=0;
int qtail=0;


//job queue
struct job *jobq[JOBQ_MAX_SIZE];

//policy variables
int jpol_id=0;
char *pol_dict[3] = {"fcfs", "sjf", "priority"};


/*
 * creates job based on given parameters and pushes it into the job queue
 */
int createq_job(char *job_name, int exec_time, int priority, int policy_id){

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
	pushq_job(new_job, policy_id);
	return 0;
}

/*
 * TODO
 * pushes the job into the job queue based off its priority
 */
int pushq_job(struct job *new_job, int policy_id){
	if (qsize == JOBQ_MAX_SIZE){
		return 0;
	}
	if (qsize == 0){
		jobq[qtail]=new_job;
		qhead=qtail;
	}
	else{
		qtail=(qtail+1)%JOBQ_MAX_SIZE;
		jobq[qtail]=new_job;
	}

	qsize++;
	return 1;
}


/*
 * returns job pointer.
 * need to call free() once finished with popped job
 */
struct job* popq_job(){
	struct job *temp;

	//return NULL if empty
	if (qsize == 0){
		temp=NULL;
		return temp;
	}

	//retrieve pointer
	temp=jobq[qhead];

	//set index to NULL
	jobq[qhead]=NULL;

	//update queue parameters
	qhead=(qhead+1)%JOBQ_MAX_SIZE;
	qsize--;

	return temp;
}

/*
 * TODO
 */
void reschedule_jobs(char *alg){

	printf("rescheduling %s\n", alg);
	return;
}

void print_jobq(){
	struct job *jp;
	if (qsize == 0){
		printf("\n");
	}
	else if (qsize == 1){
		jp=jobq[qhead];
		printf("%s\t\t%d\t\t%d\t%ld\t\t%s",
					jp->job_name,
					jp->exec_time,
					jp->priority,
					jp->arrival_time,
					"run");
	}
	else{
		int i=qhead;
		while (i != (qtail+1)%JOBQ_MAX_SIZE){
			jp=jobq[i];

			printf("%s\t\t%d\t\t%d\t%s",
						jp->job_name,
						jp->exec_time,
						jp->priority,
						asctime(localtime(&jp->arrival_time)));

			i=(i+1)%JOBQ_MAX_SIZE;
		}
		printf("\n");
	}
}

/*
 * frees the leftover memory in the job queue
 */
void free_jobq(){
	int i=qhead;
	if (qsize == 0){
		return;
	}
	else if (qsize == 1){
		free(jobq[qhead]);
	}
	else{
		while (i != qtail){
			free(jobq[i]);
			i=(i+1)%JOBQ_MAX_SIZE;
			qsize--;
		}
		free(jobq[qtail]);
		qsize--;
	}
}

int get_qsize(){
	return qsize;
}

