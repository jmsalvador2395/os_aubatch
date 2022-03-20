#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv){
	if (argc < 2){
		fprintf(stderr, "\nnot enough args. run as:\n");
		fprintf(stderr, "./batch_job <exe_time>\n\n");

		return 1;
	}
	
	//parse int
	int diff=atoi(argv[1]);

	//set start time
	time_t start, end;
	start=time(NULL);

	//set end time
	end=start+diff;
	
	//loop for [diff] seconds
	while(time(NULL) != end);

	return 0;
}
