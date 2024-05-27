CC = gcc
CFLAGS = -Wvla -Wextra -Werror

init: master atomo attivatore alimentazione inibitore library.h

library.o: library.c library.h
	$(CC) library.c $(CFLAGS) library.o -o library

master: master.c library.o
	$(CC) master.c $(CFLAGS) library.o -o master

atomo: atomo.c library.o
	$(CC) atomo.c $(CFLAGS) library.o -o atomo

attivatore: attivatore.c library.o
	$(CC) attivatore.c $(CFLAGS) library.o -o attivatore

alimentazione: alimentazione.c library.o
	$(CC) alimentazione.c $(CFLAGS) library.o -o alimentazione

inibitore: inibitore.c library.o
	$(CC) inibitore.c $(CFLAGS) library.o -o inibitore
	
run: 
	./master

clean:
	rm -f *.o