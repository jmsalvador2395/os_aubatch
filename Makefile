flags = '-lpthread'

aubatch: aubatch.o
	gcc -o aubatch aubatch.o $(flags)

aubatch.o: aubatch.c
	gcc -c aubatch.c $(flags)

clean:
	rm aubatch
	rm *.o
