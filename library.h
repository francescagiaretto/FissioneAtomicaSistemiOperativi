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

typedef struct stat_rel { // relativa è contata nel secondo
    int n_activ_rel;
    int cons_energy_rel;
    int n_div_rel;
    int prod_energy_rel;
    int prod_waste_rel;

} stat_rel;

typedef struct stat_tot { // totale è la somma delle relative fino al momento
    int n_activ_total;
    int n_div_total;
    int prod_energy_tot;
    int cons_energy_tot;
    int prod_waste_tot;

} stat_tot;

