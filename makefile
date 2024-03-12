# NON FUNZIONA ANCORA. VERSIONE VECCHIA DEL MAKEFILE IN ISSUES.TXT

init: master atomo attivatore alimentatore library.h

master: master.c library.h
	gcc master.c -Wvla -Wextra -Werror -o master

atomo: atomo.c library.h
	gcc atomo.c -Wvla -Wextra -Werror -o atomo

attivatore: attivatore.c library.h
	gcc attivatore.c -Wvla -Wextra -Werror -o attivatore

alimentatore: alimentatore.c library.h
	gcc alimentatore.c -Wvla -Wextra -Werror -o alimentatore

inibitore: inibitore.c library.h
	gcc inibitore.c -Wvla -Wextra -Werror -o inibitore
	
run: 
	./master

clean:
	rm -f *.o