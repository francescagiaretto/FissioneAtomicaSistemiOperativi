#include "library.h"

int main(int argc, char * argv[]) {

    //! bisogna passargli l'id della shared memory perché altrimenti nel vec_atomo non glielo diamo
    int atomic_num;
    char n_atom[4];
    srand(getpid());
    
    /*data_buffer * shmem_p; // inizializzi shm
    int shmid = atoi(argv[1]);
    shmem_p = (data_buffer *) shmat(shmid, NULL, 0); 
    if (shmem_p == (void *) -1) {
        perror("Pointer not attached."); exit(EXIT_FAILURE);
    }
*/

    //* STRUCT defines a nanosec-based sleep (can't be done with standard sleep())
    struct timespec step_nanosec;
    step_nanosec.tv_sec = 0;           // seconds   
    step_nanosec.tv_nsec = STEP_ALIMENTATORE;   // nanoseconds
    

    // TODO ogni STEP_ALIMENTATORE nanosecondi deve creare N_NUOVI_ATOMI
    
    while(1) {

        nanosleep(&step_nanosec, NULL); // ricontrolla bene questo, se arriva un segnale va avanti, metti conttollo che riesca a riportarti ad aspettare del tempo

        for(int i = 0; i < N_NUOVI_ATOMI; i++){
            atomic_num = rand() % RNG_N_ATOMICO + 1;
            sprintf(n_atom, "%d", atomic_num);
            char * vec_atomo[] = {"atomo", n_atom, argv[1], NULL}; // argv[1] = pointer to shmem

            switch(fork()) {

                case -1:
                    char * message = "meltdown.";
                    //termination(message, shmem_p, shmid); Mandi un segnale a master 
                    signal(SIGUSR1, term_handler)
                break;

                case 0:
                    if (execve("atomo", vec_atomo, NULL)==-1) {perror("Execve grandchild"); exit(EXIT_FAILURE);} 
                break;

                default:
                break;
            }
        }
    }
}