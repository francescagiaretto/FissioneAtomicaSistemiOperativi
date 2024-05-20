#include "library.h"

int semid, shmid, msgid;
data_buffer * shmem_ptr;

int main(char* argv[]) {

    // bisogna poter decidere a runtime se si vuole che l'inibitore sia attivo o meno 
      // (l'utente deve poter fermare e far ripartire il processo inibitore più volte da terminale)

    // non devono avvenire explode e meltdown
        // ? conseguenza naturale o va imposto?

    semid = atoi(argv[1]);
	  shmid = atoi(argv[2]);
	  msgid = atoi(argv[3]);

    shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	  TEST_ERROR;

    sem.sem_num = STARTSEM;
    sem.sem_op = -1;
    semop(semid, &sem, 1);

   /* while (shmem_ptr -> termination == 0) {
      //! assorbe un quinto dell'energia totale
      if(semctl(semid, INIBSEM, GETVAL) == 0) {
        sem.sem_num = INIBSEM;
        sem.sem_op = -1;
        semop(semid, &sem, 1);
      } else {
        shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - (shmem_ptr -> prod_en_tot / 5);
        printf("Ti fotto l'energia\n");
      }*/
    //}

   /* 
      // limita il numero di scissioni agendo sull'operazione di scissione rendendola probabilistica (decidendo se 
        // la scissione debba avvenire o meno oppure trasformando in  scoria uno degli atomi prodotti dopo la scissione)
      
        //!MANDA SEGNALE A MASTER
    } */
}