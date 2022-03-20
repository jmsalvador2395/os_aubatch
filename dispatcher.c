#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <string.h>

#include "dispatcher.h"
#include "utilities.h"

void dispatch(int time){
	int time_str_size=10;
	char time_str[time_str_size];
	itoa(time, time_str, time_str_size);

	char *args[3];
	args[0]="batch_job";
	args[1]=time_str;
	args[2]=NULL;


	pid_t pid;
	pid=fork();

	//check for failed fork
	if(pid == -1){
		fprintf(stderr, "fork() failure\n");
		exit(1);
	}

	//goes in here for child process
	if(pid == 0){
		execv("batch_job", args);
		//goes here if execv fails
		fprintf(stderr, "execv() failure\n");
		exit(1);
	}

	int status;
	wait(&status);
	
}
