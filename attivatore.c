#include "library.h"

int semid;

int main(int argc, char* argv[]) {
	
	semid = semget(atoi(argv[1]), 1, IPC_CREAT | 0666);
	if (semctl(semid, 0, SETVAL, 1) ==  -1) {
		perror("semctl in attivatore: "); exit(EXIT_FAILURE);
	}
	sem.sem_num = WAITSEM;
	sem.sem_op = 0;
	semop(semid, &sem, 1);

	write(0, "\n\n\nTEST ATTIVATORE\n\n\n", 21);
	// ? come comunicare che bisogna fare una scissione? SEGNALE

	// semaforo che controlla n atomi da decidere (possono lavorare 3 atomi insieme)
	// in caso di riuscita execvesu atomo

	/*
			Ciclo STEP_ATTIVATORE volte e ogni volta controllo se posso continuare ad attivare sennò stop
	*/

}