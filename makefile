CC = gcc
CFLAGS = -Wvla -Wextra -Werror
LIBRARY = library.h
OBJS = master.o atomo.o attivatore.o alimentazione.o inibitore.o

%.o: %.c $(LIBRARY)
	$(CC) $(CFLAGS) -c $< -o $@

master: master.c alimentazione.o attivatore.o atomo.o inibitore.o
	$(CC) $(CFLAGS) master.c -o master

alimentazione: alimentazione.c 
	$(CC) $(CFLAGS) alimentazione.c -o alimentazione

attivatore: attivatore.c
	$(CC) $(CFLAGS) attivatore.c -o attivatore

atomo: atomo.c
	$(CC) $(CFLAGS) atomo.c -o atomo

inibitore: inibitore.c
	$(CC) $(CFLAGS) inibitore.c -o inibitore

run:
	./master

clean:
	rm -f *.o