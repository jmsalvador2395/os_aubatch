#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
	printf("batch_job executing...\n");
	if (argc < 2){
		fprintf(stderr, "\nnot enough args. run as:\n");
		fprintf(stderr, "./batch_job <exe_time>\n\n");

		return 1;
	}
	
	int time=atoi(argv[1]);
	sleep(time);
	printf("batch_job exiting...\n");

	return 0;
}
