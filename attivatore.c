#include "library.h"
#define MSGYPE 1

int semid, shmid, msgid;

int main(int argc, char* argv[]) {
	
	semid = atoi(argv[1]);
	shmid = atoi(argv[2]);
	msgid = atoi(argv[3]);

  	data_buffer * shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
  	TEST_ERROR;


	struct timespec step_nanosec;
  	step_nanosec.tv_sec = 0;           // seconds   
  	step_nanosec.tv_nsec = STEP_ATTIVATORE;   // nanoseconds
	
	sem.sem_num = STARTSEM;
 	sem.sem_op = -1;
  	semop(semid, &sem, 1);
	TEST_ERROR;

	printf("ATTIVATORE: %d, shmid: %d, semid: %d\n\n", getpid(), shmid, semid);

	// ? come comunicare che bisogna fare una scissione? SEGNALE

	//! bisogna decidere il criterio per cui si continua ad attivare (es. numero max di atomi)

	/*
	Ciclo STEP_ATTIVATORE volte e ogni volta controllo se posso continuare ad attivare sennò stop

	for ( ; 1 ;) {
		for(int i = 0; i < sizeof(pid_atoms[]); i++) {
			kill(pid_atoms[i], SIGCHLD);
		}
		nanosleep(&step_nanosec, NULL);
	}
	*/

	/* altra implementazione

	int n_atoms = rand() % (pid_atoms + 1);
	for(int i = 0; i < n_atoms; i++) {
		choose a random pid_atoms[] to divide
		invia comando di scissione a pid_atoms[chosen];
		msgsnd();
	} 
	nanosleep(&step_nanosec, NULL);

	*/

	/**
	 * in ogni processo tranne master ci sarà un signal handler che gestirà l'arrivo di SIGCHLD
	 * con attivazione = 1 che sbloccherà il while su cui cicla (spreco di tempo inutile);
	*/

	// printf("\n\n\nTEST ATTIVATORE\n\n\n");

	// semaforo che controlla n atomi da decidere (possono lavorare 3 atomi insieme)
}