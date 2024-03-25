#include "library.h"

int semid, shmid, msgid;
data_buffer * shmem_ptr;

int main(char* argv[]) {

    // bisogna poter decidere a runtime se si vuole che l'inibitore sia attivo o meno 
      // (l'utente deve poter fermare e far ripartire il processo inibitore più volte da terminale)

    // non devono avvenire explode e meltdown
        // ? conseguenza naturale o va imposto?

    int inib_on = atoi(argv[1]); // values 0 and 1; 0 means off and 1 means on
    shmid = atoi(argv[2]);

    shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	  TEST_ERROR;

    if (shmem_ptr -> termination == 1) {
        int status = 0;
        waitpid(-1, &status, WIFEXITED(status));
        kill(getpid(), SIGTERM);
      }

    while (inib_on == 1) {
      
      // assorbe parte dell'energia prodotta dalla scissione dell'atomo diminuendo la quantità di energia liberata
      //shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - 20/100*(shmem_ptr -> prod_en_tot);

      // limita il numero di scissioni agendo sull'operazione di scissione rendendola probabilistica (decidendo se 
        // la scissione debba avvenire o meno oppure trasformando in  scoria uno degli atomi prodotti dopo la scissione)
      
      
    }
}