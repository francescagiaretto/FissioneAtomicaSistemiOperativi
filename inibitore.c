#include "library.h"

int semid, shmid, msgid;
data_buffer * shmem_ptr;

void signal_handler(int sig){
  switch (sig) {
    case SIGTSTP:
      if (shmem_ptr -> inib_on != 0) {
        sem.sem_num = INIBSEM;
        sem.sem_op = -1;
        semop(semid, &sem, 1);
        shmem_ptr -> inib_on = 0;
        printf("Inibitore OFF. Turn on anytime with ctrl + '\' \n");
      }
    break;
  }
}

int main(int argc, char* argv[]) {

    // bisogna poter decidere a runtime se si vuole che l'inibitore sia attivo o meno 
      // (l'utente deve poter fermare e far ripartire il processo inibitore più volte da terminale)

    // non devono avvenire explode e meltdown
        // ? conseguenza naturale o va imposto?

    semid = atoi(argv[1]);
	  shmid = atoi(argv[2]);
	  msgid = atoi(argv[3]);

    shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	  TEST_ERROR;

    struct sigaction sa;

    bzero(&sa, sizeof(&sa)); 
    sa.sa_handler = &signal_handler;
    sigaction(SIGTSTP, &sa, NULL);

    sem.sem_num = STARTSEM;
    sem.sem_op = -1;
    semop(semid, &sem, 1);

    shmem_ptr -> inib_on = 0;
    //! potrebbe dare problemi
    sem.sem_num = INIBSEM;
    sem.sem_op = -1;
    semop(semid, &sem, 1);

    while (shmem_ptr -> termination == 0) {
      //! assorbe un quinto dell'energia totale
      sleep(0.0001);
      shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - (shmem_ptr -> prod_en_tot / 5);
      printf("Ti fotto l'energia\n");
    }

   /* 
      // limita il numero di scissioni agendo sull'operazione di scissione rendendola probabilistica (decidendo se 
        // la scissione debba avvenire o meno oppure trasformando in  scoria uno degli atomi prodotti dopo la scissione)
      
        //!MANDA SEGNALE A MASTER
    } */
}