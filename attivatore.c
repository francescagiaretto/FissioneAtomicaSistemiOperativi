#include "library.h"

int semid, shmid;

int main(int argc, char* argv[]) {
	
	semid = atoi(argv[1]);
	/* semid = semget(atoi(argv[1]), 2, IPC_CREAT | 0666);
	TEST_ERROR; */

	shmid = atoi(argv[2]);
  	data_buffer * shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
  	TEST_ERROR;

	if (shmem_ptr -> termination == 1) {
		int status = 0;
		waitpid(-1, &status, WIFEXITED(status));
		kill(getpid(), SIGTERM);
	}
	
	sem.sem_num = STARTSEM;
 	sem.sem_op = -1;
  	semop(semid, &sem, 1);
	TEST_ERROR;

	for(; 1; ) {
		
	}
	// printf("\n\n\nTEST ATTIVATORE\n\n\n");
	// ? come comunicare che bisogna fare una scissione? SEGNALE

	// semaforo che controlla n atomi da decidere (possono lavorare 3 atomi insieme)
	// in caso di riuscita execvesu atomo

	/*
			Ciclo STEP_ATTIVATORE volte e ogni volta controllo se posso continuare ad attivare sennò stop
	*/

}