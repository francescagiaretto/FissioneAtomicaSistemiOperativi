#include "library.h"

int semid, shmid, msgid;
data_buffer * shmem_ptr;

void signal_handler(int sig){
  switch (sig) {
    case  SIGINT:
      if (shmem_ptr -> inib_on != 0) {
        write(0, "Inibitore OFF. Turn off anytime with ctrl + backslash \n", 55);
        /* sem.sem_num = ONSEM;
        sem.sem_op = -1;
        semop(semid, &sem, 1); */

        shmem_ptr -> inib_on = 0;

        /* sem.sem_num = ONSEM;
        sem.sem_op = 1;
        semop(semid, &sem, 1); */
        //printf("inib_on inibitore = %d\n", shmem_ptr -> inib_on);
        sem.sem_num = INIBSEM;
        sem.sem_op = -1;
        semop(semid, &sem, 1);
      }
    break;
  }
}

int main(int argc, char* argv[]) {

    // bisogna poter decidere a runtime se si vuole che l'inibitore sia attivo o meno 
      // (l'utente deve poter fermare e far ripartire il processo inibitore più volte da terminale)

    // non devono avvenire explode e meltdown
        // ? conseguenza naturale o va imposto

    semid = atoi(argv[1]);
	  shmid = atoi(argv[2]);
	  msgid = atoi(argv[3]);

    shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	  TEST_ERROR;

    struct sigaction sa;

    bzero(&sa, sizeof(sa)); 
    sa.sa_handler = &signal_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    shmem_ptr -> inib_on = 0;

    printf("%d in inibitore subito prima\n", semctl(semid, INIBSEM, GETVAL));
    sem.sem_num = INIBSEM;
    sem.sem_op = -1;
    semop(semid, &sem, 1);
    printf("%d in inibitore subito dopo\n", semctl(semid, INIBSEM, GETVAL));

    sem.sem_num = STARTSEM;
    sem.sem_op = -1;
    semop(semid, &sem, 1);

    while (shmem_ptr -> termination == 0) {
      //! assorbe un quinto dell'energia totale
      sleep(1);
      shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - (shmem_ptr -> prod_en_tot / 5);
      //printf("%d è inib_on da attivo, ", shmem_ptr -> inib_on);
      //printf("Ti fotto l'energia\n");
    }

      // limita il numero di scissioni agendo sull'operazione di scissione rendendola probabilistica (decidendo se 
        // la scissione debba avvenire o meno oppure trasformando in  scoria uno degli atomi prodotti dopo la scissione)
      
        //!MANDA SEGNALE A MASTER
}