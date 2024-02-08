// progetto Sistemi Operativi: Cavallo Alessandra, Fistos Andrei, Faraca Niccolò.

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#define N_ATOM_INIT 5
#define ENERGY_DEMAND 2000
#define SIM_DURATION 30
#define ENERGY_EXPLODE_THRESHOLD 200000000


void print_stats(stats);
int stat_total_value(int *, int *);

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

void signal_handler(int sig) {  // gestisce il segnale di sigalarm che arriva dall'alarm(30), che definisce la terminazione "timeout"
    write(0, "timeout.\n", 9);
    exit(0);
}

int main(int argc, char* argv[]) {
    pid_t pid_alimentatore, pid_attivatore;     // pid dei processi alimentatore e attivatore
    pid_t * pid_atomi;
    stats stat = {0};   // inizializzo la struct a 0

    srand(getppid());
    int random = (rand() % 70) + 1;
    char * num_atomico = (char *)&random; 
    char * vec_atomo[] = {};
    char * vec_alim[] = {};
    char * vec_attiv[] = {num_atomico};

    // creazione processo attivatore e alimentatore
    switch(pid_alimentatore = fork()) {
        // controlla se c'è meltdown

        case -1:
            printf("meltdown."); exit(EXIT_FAILURE);

        case 0:
        if(execve("alimentatore", vec_alim, NULL) == -1) {perror("execve"); exit(EXIT_FAILURE);}
        break;

        default:

        switch(pid_attivatore = fork()) {

            case -1:
            printf("meltdown.");
            exit(EXIT_FAILURE);

            case 0:
            if(execve("attivatore", vec_attiv, NULL) == -1) { perror("execve"); exit(EXIT_FAILURE);} 
            break;
        }
        break;
    }

    pid_atomi = malloc(sizeof(pid_t) * N_ATOM_INIT);    // allocazione dinamica di memoria per l'array di pid atomi

    // creazione processi atomo
    for(int i = 0; i < N_ATOM_INIT; i++) {
        pid_atomi[i] = fork();

        switch(pid_atomi[i]) {

            case -1:
            printf("meltdown.");
            exit(EXIT_FAILURE);

            case 0:
            free(pid_atomi);
            if (execve("atomo", vec_atomo, NULL) == -1) {perror("execve"); exit(EXIT_FAILURE);}  // non funziona
            break;

            default:
            printf("atomo con pid %d\n", pid_atomi[i]);
            break;
        }
    }

    // solo quando tutto è in regola posso far partire la simulazione, che dura 30 secondi 
    struct sigaction sa;

    bzero(&sa, sizeof(&sa));
    sa.sa_handler = &signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    alarm(5);
    for(; 1; ) {
        print_stats(stat);
        
        sleep(1);
    }
}

void print_stats(stats stat) {
    static int count = 0;

    printf("\n\n\n\n");
    printf("STATISTICHE RELATIVE ALL'ULTIMO SECONDO:\n");
    printf("- numero attivazioni:   %d\n", stat.n_activ_rel);
    printf("- numero scissioni:     %d\n", stat.n_div_rel);
    printf("- numero enegia prodotta:       %d\n", stat.prod_energy_rel);
    printf("- numero energia consumata:     %d\n", stat.cons_energy_rel);
    printf("- numero scorie prodotte:       %d\n", stat.prod_waste_rel);
    
    printf("\n");

    printf("STATISTCHE TOTALI:\n");
    printf("- numero attivazioni: %d\n", stat_total_value(&stat.n_activ_total, &stat.n_activ_rel));
    printf("- numero scissioni: %d\n", stat_total_value(&stat.n_div_total, &stat.n_div_rel));
    printf("- numero enegia prodotta: %d\n", stat_total_value(&stat.prod_energy_tot, &stat.prod_energy_rel));
    printf("- numero energia consumata: %d\n", stat_total_value(&stat.cons_energy_tot, &stat.cons_energy_rel));
    printf("- numero scorie prodotte: %d\n", stat_total_value(&stat.prod_waste_tot, &stat.prod_energy_rel));

    printf("%d\n", count++);
    
}

int stat_total_value(int * tot_value, int * rel_value) {
        *tot_value += *rel_value;
        return *tot_value;
    }