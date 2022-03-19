#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#define NAME_LEN 		30
#define POL_NAME_LEN	8
#define MIN_CPU_TM		3
#define MAX_CPU_TM		4

struct test_args{
	char bench_name[NAME_LEN];
	char policy[POL_NAME_LEN];
	int num_jobs;
	int priority;
	int min_cpu_time;
	int max_cpu_time;
};

struct run_args{
	char job_name[NAME_LEN];
	int time;
	int priority;
};


int parse_test_args(struct test_args *t_args, char *input);
int parse_run_args(struct run_args *r_args, char *input);

#endif
