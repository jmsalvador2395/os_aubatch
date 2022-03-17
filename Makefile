flags = -lpthread -lm
CC = gcc
aub_outputs=aubatch.o dispatcher.o utilities.o job_utils.o
bj_outputs=batch_job.o


aubatch: $(aub_outputs) $(bj_outputs)
	$(CC) -o aubatch $(aub_outputs) $(flags)
	$(CC) -o batch_job $(bj_outputs)

batch_job.o: batch_job.c
	$(CC) -c batch_job.c

aubatch.o: aubatch.c
	$(CC) -c aubatch.c $(flags)

dispatcher.o: dispatcher.c
	$(CC) -c dispatcher.c $(flags)

utilities.o: utilities.c
	$(CC) -c utilities.c $(flags)

job_utils.o: job_utils.c
	$(CC) -c job_utils.c $(flags)

clean: *.o
	rm *.o
	rm batch_job
	rm aubatch
