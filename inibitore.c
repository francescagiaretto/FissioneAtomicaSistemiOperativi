#include "library.h"

int semid, shmid, msgid;
data_buffer * shmem_ptr;

void signal_handler(int sig){
  switch (sig) {
    case SIGQUIT:
      if (shmem_ptr -> inib_on != 0) {
        write(0, "Inibitore OFF. Turn off anytime with ctrl + backslash \n", 55);

        shmem_ptr -> inib_on = 0;
        
      } else if (shmem_ptr -> inib_on == 0) {
        //! generiamo un numero randomico tra 0 e 1; se esce 0 l'attivatore uccide i pari, se esce 1 i dispari
        shmem_ptr -> remainder = rand() % 2;
        
        kill(shmem_ptr -> pid_master, SIGUSR1);
      }
    break;
  }
}

int main(int argc, char* argv[]) {

    // bisogna poter decidere a runtime se si vuole che l'inibitore sia attivo o meno 
      // (l'utente deve poter fermare e far ripartire il processo inibitore più volte da terminale)

    // non devono avvenire explode e meltdown

  semid = atoi(argv[1]);
	shmid = atoi(argv[2]);
	msgid = atoi(argv[3]);

  shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

  srand(getpid());

  struct sigaction sa;
  bzero(&sa, sizeof(sa)); 
  sa.sa_handler = &signal_handler;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGQUIT, &sa, NULL);

  shmem_ptr -> inib_on = 0;

  sem.sem_num = STARTSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);

  sem.sem_num = INIBSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);

  while (shmem_ptr -> termination == 0) {
      	sleep(0.5);
	
      	//! se l'inibitore viene disattivato il semaforo blocca la stampa dell'energia evitando l'attesa attiva
      	if(shmem_ptr -> inib_on == 0) {
      	  sem.sem_num = INIBSEM;
      	  sem.sem_op = -1;
      	  semop(semid, &sem, 1);
      	}
	
      	//! assorbe un quinto dell'energia totale
      	shmem_ptr -> absorbed_en_rel = (shmem_ptr -> prod_en_tot)/5;
      	shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - shmem_ptr -> absorbed_en_rel;
	
    		// limita il numero di scissioni agendo sull'operazione di scissione rendendola probabilistica (decidendo se 
      	// la scissione debba avvenire o meno oppure trasformando in  scoria uno degli atomi prodotti dopo la scissione)
    }
}