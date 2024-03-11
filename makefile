# NON FUNZIONA ANCORA. VERSIONE VECCHIA DEL MAKEFILE IN ISSUES.TXT

CC = gcc
CFLAGS = -Wvla -Wextra -Werror
INCLUDES = library.h
DEPENDENCIES = $(INCLUDES) folders

init: code/methods code/master code/atomo code/attivatore code/alimentatore code/inibitore $(INCLUDES)

run: code/master

folders:
	mkdir -p object/ code/

#code/%.c: ./%.c $(DEPENDENCIES)
#$(CC) $(CFLAGS) -c $< -o $@

object/%.o: /%.c $(DEPENDENCIES)
	$(CC) $(CFLAGS) -c $< -o $@


code/master: object/master.o $(DEPENDENCIES)
	$(CC) -o code/master

code/atomo: object/atomo.o $(DEPENDENCIES)
	$(CC) -o code/atomo

code/attivatore: object/attivatore.o $(DEPENDENCIES)
	$(CC) -o code/attivatore
	
code/alimentatore: object/alimentatore.o $(DEPENDENCIES)
	$(CC) -o code/alimentatore

code/inibitore: object/inibitore.o $(DEPENDENCIES)
	$(CC) -o code/inibitore

code/methods: object/methods.o $(DEPENDENCIES)
	$(CC) -o code/methods

clean:
	rm -rf object/ code/