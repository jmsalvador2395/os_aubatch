#ifndef JOB_UTILS_H
#define JOB_UTILS_H

#define JOBQ_MAX_SIZE 64


struct job{

	char job_name[30];
	int exec_time;
	int priority;
	time_t arrival_date;
	struct timeval arrival_time;
	int status;
	
};

struct job_vertex{
	struct job j;
	struct job_vertex *next;
	struct job_vertex *prev;
	
};

int sched_id;
int queue_size;
int list_size;
struct job *jobq[JOBQ_MAX_SIZE];
int createq_job(char *job_name, int exec_time, int priority, int policy_id);
int pushq_job(struct job *new_job, int policy_id);
struct job* popq_job();
void reschedule_jobs(int policy_id);
void print_jobq();
void free_jobq();
int get_qsize();
int less_than(struct job *j1, struct job *j2, int policy_id);
int greater_than(struct job *j1, struct job *j2, int policy_id);
void jq_print_seq();
double ms_time_diff(struct timeval *t1, struct timeval *t2);
int wait_time(int placement);
	

#endif
