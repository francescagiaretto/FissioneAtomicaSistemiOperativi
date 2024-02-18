#include "library.h"

int main() {

    // STRUCT per definire una sleep contata in nanosecondi, non possibile usando una sleep() normale.
    struct timespec step_nanosec;
    step_nanosec.tv_sec = 0;            
    step_nanosec.tv_nsec = STEP_ALIMENTATORE;

    // TODO ogni STEP_ALIMENTATORE nanosecondi deve creare N_NUOVI_ATOMI

    /* while(1) {
        nanosleep(&step_nanosec, NULL);

        for(int i = 0; i < N_NUOVI_ATOMI; i++){
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