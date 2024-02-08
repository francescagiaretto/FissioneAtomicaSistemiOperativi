init : 
	gcc -c master.c
	gcc master.o -o master
	gcc -c atomo.c
	gcc atomo.o -o atomo
	gcc -c attivatore.c
	gcc attivatore.o -o attivatore
	gcc -c alimentatore.c
	gcc alimentatore.o -o alimentatore
	
run: 
	./master

clean:
	rm -f *.o