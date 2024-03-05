#include "library.h"

int main(int argc, char * argv[]) {

    //! bisogna passargli l'id della shared memory perché altrimenti nel vec_atomo non glielo diamo
    int shmid = atoi(argv[1]); int n_atomico;
    char n_atom[4];
    srand(getpid());

    //* STRUCT per definire una sleep contata in nanosecondi, non possibile usando una sleep() normale.
    struct timespec step_nanosec;
    step_nanosec.tv_sec = 0;           // secondi    
    step_nanosec.tv_nsec = STEP_ALIMENTATORE;   // nanosecondi

    // TODO ogni STEP_ALIMENTATORE nanosecondi deve creare N_NUOVI_ATOMI
    
    /* while(1) {
        nanosleep(&step_nanosec, NULL);

        for(int i = 0; i < N_NUOVI_ATOMI; i++){
            n_atomico = rand() % RNG_NUM_ATOMICO + 1;
            sprintf(n_atom, "%d", n_atomico);
            char * vec_atomo = {"atomo", n_atomico, shmid, NULL}

            switch(fork()) {

                case -1:
                    printf("Simulazione terminata: meltdown.");
                    exit(EXIT_FAILURE);
                break;

                case 0:
                    if (execve("atomo", vec_atomo, NULL)==-1) {perror("Execve nipote"); exit(EXIT_FAILURE);} 
                break;

                default:
                break;
            }
        }
    } */
}