#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define PLCY 		0
#define NUM_JOBS 	1
#define PRIORITIES	2
#define MIN_CPU_TM	3
#define MAX_CPU_TM	4

/*
 * args :
 * <policy>
 * <num_of_jobs>
 * <priority_levels>
 * <min_CPU_time>
 * <max_CPU_time>
 */
int main(int argc, char **argv){

	//return error if not enough args
	if(argc < 6){
		fprintf(stderr, "\nnot enough arguements. run as:\n");
		fprintf(stderr, "./aubatch <policy> <num_jobs> <priority_levels> <min_cpu_time> <max_cpu_time>\n\n");
		return 1;
	}

	//args
	char *policy;
	char *num_jobs;
	char *priority_lvs;
	char *min_cpu_time;
	char *max_cpu_time;


	//create array of lengths of the args
	int i;
	int lens[5]={0};
	int len;
	for(i=0; i<5; i++){
		lens[i]=strlen(argv[i+1]);
	}

	//allocate memory for the arguements
	policy		 =malloc(sizeof(char)*lens[0]+1);
	num_jobs	 =malloc(sizeof(char)*lens[1]+1);
	priority_lvs =malloc(sizeof(char)*lens[2]+1);
	min_cpu_time =malloc(sizeof(char)*lens[3]+1);
	max_cpu_time =malloc(sizeof(char)*lens[4]+1);

	//copy args over
	strcpy(policy		, argv[1]);
	strcpy(num_jobs		, argv[2]);
	strcpy(priority_lvs	, argv[3]);
	strcpy(min_cpu_time	, argv[4]);
	strcpy(max_cpu_time	, argv[5]);


	//free args
	free(policy);
	free(num_jobs);
	free(priority_lvs);
	free(min_cpu_time);
	free(max_cpu_time);
	
	return 0;
}
