oflags = -lpthread -lm
cflags = -lpthread
CC = gcc

aubatch: aubatch.o dispatcher.o utilities.o batch_job.o
	$(CC) -o aubatch aubatch.o dispatcher.o utilities.o $(oflags)
	$(CC) -o batch_job batch_job.o

batch_job.o: batch_job.c
	$(CC) -c batch_job.c

aubatch.o: aubatch.c
	$(CC) -c aubatch.c $(cflags)

dispatcher.o: dispatcher.c
	$(CC) -c dispatcher.c $(oflags)

utilities.o: utilities.c
	$(CC) -c utilities.c $(cflags)

clean: *.o
	rm *.o
	rm aubatch;
	rm batch_job
