CC = gcc
CFLAGS = -Wvla -Wextra -Werror

init: master atomo attivatore alimentazione inibitore library.h

master: master.c library.h
	$(CC) master.c $(CFLAGS) -o master

atomo: atomo.c library.h
	$(CC) atomo.c $(CFLAGS) -o atomo

attivatore: attivatore.c library.h
	$(CC) attivatore.c $(CFLAGS) -o attivatore

alimentazione: alimentazione.c library.h
	$(CC) alimentazione.c $(CFLAGS) -o alimentazione

inibitore: inibitore.c library.h
	$(CC) inibitore.c $(CFLAGS) -o inibitore
	
run: 
	./master

clean:
	rm -f *.o