#include "library.h"

int main(int argc, char * argv[]) {

    //! bisogna passargli l'id della shared memory perché altrimenti nel vec_atomo non glielo diamo
    int shmid = atoi(argv[1]); int atom_num;
    char n_atom[4];
    srand(getpid());

    //* STRUCT defines a nanosec-based sleep (can't be done with standard sleep())
    struct timespec step_nanosec;
    step_nanosec.tv_sec = 0;           // seconds   
    step_nanosec.tv_nsec = STEP_ALIMENTATORE;   // nanoseconds

    // TODO ogni STEP_ALIMENTATORE nanosecondi deve creare N_NUOVI_ATOMI
    
    /* while(1) {
        nanosleep(&step_nanosec, NULL);

        for(int i = 0; i < N_NUOVI_ATOMI; i++){
            atom_num = rand() % RNG_NUM_ATOMICO + 1;
            sprintf(n_atom, "%d", atom_num);
            char * vec_atomo = {"atomo", atom_num, shmid, NULL}

            switch(fork()) {

                case -1:
                    printf("Simulation terminated due to meltdown.\n");
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