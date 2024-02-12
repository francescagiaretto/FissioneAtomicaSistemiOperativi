#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "parameters.txt"

typedef struct stats {
    int n_activ_rel;    // relativa è contata nel secondo
    int n_activ_total;  // totale è la somma delle relative fino al momento
    int n_div_rel;
    int n_div_total;
    int prod_energy_rel;
    int prod_energy_tot;
    int cons_energy_rel;
    int cons_energy_tot;
    int prod_waste_rel;
    int prod_waste_tot;

} stats;