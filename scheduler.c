#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <string.h>

#include "scheduler.h"

void *schedule(void *args){
	char *args[3];
	args[0]="batch_job";
	args[1]="2";
	args[2]=NULL;

	int tm=(long) time;

	//printf("%d\n", tm);

	pid_t pid;

	pid=fork();

	//check for failed fork
	if(pid == -1){
		fprintf(stderr, "fork() failure\n");
		exit(1);
	}

	//goes in here for child process
	if(pid == 0){
		printf("dispatching...\n");
		execv("batch_job", args);
		//goes here if execv fails
		fprintf(stderr, "execv() failure\n");
		exit(1);
	}

	int status;
	wait(&status);

	
}
