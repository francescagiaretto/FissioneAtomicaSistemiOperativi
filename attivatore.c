#include "library.h"

int semid, shmid;

int main(int argc, char* argv[]) {
	
	semid = atoi(argv[1]);
	/* semid = semget(atoi(argv[1]), 2, IPC_CREAT | 0666);
	TEST_ERROR; */

	// passiamo array con pid atomi = pid_atoms[]

	shmid = atoi(argv[2]);
  	data_buffer * shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
  	TEST_ERROR;

	if (shmem_ptr -> termination == 1) {
		kill(getpid(), SIGTERM);
	}
	
	sem.sem_num = STARTSEM;
 	sem.sem_op = -1;
  	semop(semid, &sem, 1);
	TEST_ERROR;

	// ? come comunicare che bisogna fare una scissione? SEGNALE

	//! bisogna decidere il criterio per cui si continua ad attivare (es. numero max di atomi)

	/*
	Ciclo STEP_ATTIVATORE volte e ogni volta controllo se posso continuare ad attivare sennò stop

	for ( ; 1 ;) {
		for(int i = 0; i < sizeof(pid_atoms[]); i++) {
			kill(pid_atoms[i], SIGCHLD);
		}
		sleep(STEP_ATTIVATORE);
	}
	*/

	// printf("\n\n\nTEST ATTIVATORE\n\n\n");

	// semaforo che controlla n atomi da decidere (possono lavorare 3 atomi insieme)
	// in caso di riuscita execvesu atomo
}