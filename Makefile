flags = -lpthread -lm
CC = gcc
aub_outputs=aubatch.o dispatcher.o utilities.o job_queue.o
bj_outputs=batch_job.o


aubatch: $(aub_outputs) $(bj_outputs)
	#$(CC) -o aubatch aubatch.o dispatcher.o utilities.o job_queue.o $(flags)
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

job_queue.o: job_queue.c
	$(CC) -c job_queue.c $(flags)

clean: *.o
	rm *.o
	rm batch_job
	rm aubatch
