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


/*
 * creates job based on given parameters and pushes it into the job queue
 */
int createq_job(char *job_name, int exec_time, int priority){

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
	pushq_job(new_job);
	return 0;
}

int pushq_job(struct job *new_job){
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

/*
strcpy(new_job->job_name, job_name);
new_job->exec_time=exec_time;
new_job->priority=priority;
new_job->arrival_time=time(NULL);
new_job->status=0;
*/


void print_jobq(){
	if (qsize == 0){
		printf("No jobs to display\n");
	}
	else if (qsize == 1){
		printf("\n%s\n", jobq[qhead]->job_name);
	}
	else{
		int i=qhead;
		printf("\n");
		while (i != qtail){
			printf("%s\n", jobq[qhead]->job_name);
			i=(i+1)%JOBQ_MAX_SIZE;
		}
		printf("%s\n", jobq[qtail]->job_name);
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

