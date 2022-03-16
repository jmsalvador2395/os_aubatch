flags = -lpthread -lm
CC = gcc

aubatch: aubatch.o dispatcher.o utilities.o batch_job.o
	$(CC) -o aubatch aubatch.o dispatcher.o utilities.o $(flags)
	$(CC) -o batch_job batch_job.o

batch_job.o: batch_job.c
	$(CC) -c batch_job.c

aubatch.o: aubatch.c
	$(CC) -c aubatch.c $(flags)

dispatcher.o: dispatcher.c
	$(CC) -c dispatcher.c $(flags)

utilities.o: utilities.c
	$(CC) -c utilities.c $(flags)

clean: *.o
	rm *.o
	rm batch_job
	rm aubatch
