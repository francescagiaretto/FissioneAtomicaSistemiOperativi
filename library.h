#define _GNU_SOURCE
#define MAX(a, b) ((a > b) ? (a) : (b))
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "parameters.txt"
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>

typedef struct stat_rel { // last second stats
    int n_activ_rel;
    int cons_energy_rel;
    int n_div_rel;
    int prod_energy_rel;
    int prod_waste_rel;

} stat_rel;

typedef struct stat_tot { // adds up relative stats
    int n_activ_total;
    int n_div_total;
    int prod_energy_tot;
    int cons_energy_tot;
    int prod_waste_tot;

} stat_tot;

typedef struct data_buffer {
    int position;
    int data[10];
} data_buffer;

void termination(char * message, int shmid, data_buffer * shmem_p) {
    printf("Simulation terminated due to %s\n", message);
    fflush(stdout);
    shmdt(&shmem_p);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}