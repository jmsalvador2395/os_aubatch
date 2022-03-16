#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#define QUEUE_SIZE 64
struct job{

	char job_name[30];
	int exec_time;
	int priority;
	time_t arrival_time;
	int status;
	
};

struct job *jobq[QUEUE_SIZE];
int create_job(char *job_name, int exec_time, int priority);
int push_job(struct job *new_job);
struct job *pop_job();
void reschedule_jobs(char *alg);

#endif
