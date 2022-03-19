#ifndef JOB_UTILS_H
#define JOB_UTILS_H

#define JOBQ_MAX_SIZE 64
struct job{

	char job_name[30];
	int exec_time;
	int priority;
	time_t arrival_time;
	int status;
	
};

struct job_vertex{
	struct job j;
	struct job_vertex *next;
	struct job_vertex *prev;
	
};

int queue_size;
int list_size;
struct job *jobq[JOBQ_MAX_SIZE];
int createq_job(char *job_name, int exec_time, int priority);
int pushq_job(struct job *new_job);
struct job* popq_job();
void reschedule_jobs(char *alg);
void print_jobq();
void free_jobq();
int get_qsize();

#endif
