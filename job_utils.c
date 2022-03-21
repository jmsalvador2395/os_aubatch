#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

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
	new_job->arrival_date=time(NULL);
	gettimeofday(&new_job->arrival_time, NULL);
	new_job->status=0;

	//push job into queue
	int placement=pushq_job(new_job, policy_id);
	return placement;
}

/*
 * 
 * pushes the job into the job queue based off its priority
 */
int pushq_job(struct job *new_job, int policy_id){
	if (qsize == JOBQ_MAX_SIZE){
		return -1;
	}
	if (qsize == 0){
		jobq[qtail]=new_job;
		qhead=qtail;
		qsize++;
		return 0;
	}
	else{
		int placed=0;
		int placement;
		int i, count;
		count=0;
		struct job *temp1;
		struct job *temp2;
		temp1=new_job;
		i=qhead;
		while (count < qsize){
			if (less_than(temp1, jobq[i], policy_id)){
				if(placed==0){
					placed=1;
					placement=count;
				}
				temp2=jobq[i];
				jobq[i]=temp1;
				temp1=temp2;
			}
			i=(i+1)%JOBQ_MAX_SIZE;
			count++;
		}
		if (placed == 0) placement=count;
		jobq[i]=temp1;
		qtail=i;
		qsize++;
		return placement;
	}
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
	if(qsize==1){
		qtail=qhead;
	}
	qsize--;

	return temp;
}

/*
 * TODO
 */
void reschedule_jobs(int policy_id){
	if (qsize <= 1){
		return;
	}
	else{
		//run insertion sort
		int i, j;
		int n=JOBQ_MAX_SIZE;
		struct job *key;
		for (i=1; i < n; i++){
			
			key=jobq[i];
			j=i-1;

			while(j>=0 && greater_than(jobq[j], key, policy_id)){
				jobq[j+1]=jobq[j];
				j=j-1;
			}
			jobq[j+1]=key;
		}
		qhead=0;
		qtail=qsize-1;
	}
	return;
}


void print_jobq(){
	struct job *jp;
	if (qsize == 0){
		printf("\n");
	}
	else{
		int i=qhead;
		while (i != (qtail+1)%JOBQ_MAX_SIZE){
			jp=jobq[i];

			printf("%s\t\t%d\t\t%d\t%s",
						jp->job_name,
						jp->exec_time,
						jp->priority,
						asctime(localtime(&jp->arrival_date)));

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
	qhead=0;
	qtail=0;
}

int get_qsize(){
	return qsize;
}

int less_than(struct job *j1, struct job *j2, int policy_id){
	if (policy_id == 0){
		return (j1->arrival_date < j2->arrival_date);
	}
	else if (policy_id == 1){
		return (j1->exec_time < j2->exec_time);
	}
	else if (policy_id == 2){
		return (j1->priority < j2->priority);
	}
}

int greater_than(struct job *j1, struct job *j2, int policy_id){
	if (j1 == NULL && j2 == NULL){
		return 0;
	}
	else if (j1 == NULL && j2 != NULL){
		return 1;
	}
	else if (j1 != NULL && j2 == NULL){
		return 0;
	}
	else if (policy_id == 0){
		return (j1->arrival_date > j2->arrival_date);
	}
	else if (policy_id == 1){
		return (j1->exec_time > j2->exec_time);
	}
	else if (policy_id == 2){
		return (j1->priority > j2->priority);
	}
}

void jq_print_seq(){
	int n=JOBQ_MAX_SIZE;
	int i;
	printf("qhead: %d, qtail: %d\n", qhead, qtail);
	for (i=0; i < n; i++){
		if (jobq[i] != NULL){
			printf("idx: %d, name: %s\n", i, jobq[i]->job_name);
		}
	}

}

double ms_time_diff(struct timeval *t1, struct timeval *t2){
	double sec_diff=difftime(t2->tv_sec, t1->tv_sec);
	double ms_diff=sec_diff*1000;
	ms_diff+=(t2->tv_usec-t1->tv_usec)/1000;
	return ms_diff;
}

int wait_time(int placement){
	if (qsize == 0){
		return 0;
	}
	int wait_time=0;
	int count=0;
	int i=qhead;
	while (count < placement){
		wait_time+=jobq[i]->exec_time;
		i=(i+1)%JOBQ_MAX_SIZE;
		count++;
	}
	return wait_time;
}
