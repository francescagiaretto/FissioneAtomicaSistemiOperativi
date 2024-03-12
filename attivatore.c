#include "library.h"

int semid;

int main(int argc, char* argv[]) {
	
	if (semid = semget(atoi(argv[1]), 2, IPC_CREAT | 0666) == -1) {
		perror("semget in attivatore failed"); exit(EXIT_FAILURE);
	}
	/*if (semctl(semid, 0, SETVAL, 0) ==  -1) {
		perror("semctl in attivatore failed!"); exit(EXIT_FAILURE);
	}*/
	/* sem.sem_num = STARTSEM;
 	sem.sem_op = -1;
  	if (semop(semid, &sem, 1) == -1){perror("semop startsem in attivatore"); exit(EXIT_FAILURE);} */

	// printf("\n\n\nTEST ATTIVATORE\n\n\n");
	// ? come comunicare che bisogna fare una scissione? SEGNALE

	// semaforo che controlla n atomi da decidere (possono lavorare 3 atomi insieme)
	// in caso di riuscita execvesu atomo

	/*
			Ciclo STEP_ATTIVATORE volte e ogni volta controllo se posso continuare ad attivare sennò stop
	*/

}