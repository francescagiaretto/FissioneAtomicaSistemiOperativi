#include "library.h"
#include <time.h>

// ? come giostrare i metodi nella libreria ?

void print_stats(stats);
int stat_total_value(int *, int *);

void signal_handler(int sig) {  // gestisce il segnale di sigalarm che arriva dall'alarm(30), che definisce la terminazione "timeout"
    write(0, "timeout.\n", 10);
    exit(0);
}

int main(int argc, char* argv[]) {

    // TODO controllare energy_explode_threshold

    pid_t pid_alimentatore, pid_attivatore;     // pid dei processi alimentatore e attivatore
    pid_t * pid_atomi;
    stats stat = {0};   // inizializzo la struct a 0

    char * vec_alim[] = {"alimentatore", NULL};
    char * vec_attiv[] = {"attivatore", NULL};


    // creazione processo attivatore e alimentatore
    switch(pid_alimentatore = fork()) {
        // controlla se c'è meltdown

        case -1:
            printf("meltdown."); exit(EXIT_FAILURE);

        case 0:
        if(execve("alimentatore", vec_alim, NULL) == -1) {perror("execve alim"); exit(EXIT_FAILURE);}
        break;

        default: // siamo nel processo padre

        switch(pid_attivatore = fork()) {

            case -1:
            printf("meltdown.");
            exit(EXIT_FAILURE);

            case 0:
            if(execve("attivatore", vec_attiv, NULL) == -1) { perror("execve attiv"); exit(EXIT_FAILURE);} 
            break;
        }
        break;
    }

    pid_atomi = malloc(sizeof(pid_t) * N_ATOM_INIT);    // allocazione dinamica di memoria per l'array di pid atomi

    // creazione processi atomo
    for(int i = 0; i < N_ATOM_INIT; i++) {
        pid_atomi[i] = fork();

        int range = 118 - 90 + 1; // numero atomico compreso tra 118 (max tavola periodica) e 90 (min per fissione)

        srand(time(NULL));
        int num_atomico = rand() % range + 90;
        char * vec_atomo[] = {"atomo", (char*)&num_atomico, NULL};

        switch(pid_atomi[i]) {

            case -1:
            printf("meltdown.");
            exit(EXIT_FAILURE);

            case 0: // caso figli: libero la memoria allocata con la malloc
            free(pid_atomi);

            if (execve("atomo", vec_atomo, NULL) == -1) {perror("execve atomi"); exit(EXIT_FAILURE);}  // non funziona
            break;

            default:
            printf("atomo con pid %d\n", pid_atomi[i]);
            break;
        }
    }

    // solo quando tutto è in regola posso far partire la simulazione, che dura 30 secondi 
    struct sigaction sa;

    bzero(&sa, sizeof(&sa)); // azzera tutta la struct per passarla al figlio
    sa.sa_handler = &signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    free(pid_atomi);

    alarm(5);
    for(; 1; ) {
        print_stats(stat);
        
        sleep(1);
    }

    // TODO liberare memoria a simulazione terminata
}

void print_stats(stats stat) {
    static int count = 0;

    int col_width = 40;
    printf("\n\n\n\n");
    printf("STATISTICHE:\n");
    printf("%-*s | %-*s\n", col_width, "Tipo", col_width, "Valore");
    printf("%-*c | %-*c\n", col_width, '-', col_width, '-');

    char *col1[11] = {"Attivazioni ultimo secondo:","Attivazioni totali:","Scissioni ultimo secondo:","Scissioni totali:",
        "Energia prodotta ultimo secondo:","Energia prodotta totale:","Energia consumata ultimo secondo:","Energia consumata totale:",
        "Scorie prodotte ultimo secondo:","Scorie prodotte totali:"};
    int col2[11] = {stat.n_activ_rel,stat.n_activ_total,stat.n_div_rel,stat.n_div_total,stat.prod_energy_rel,stat.prod_energy_tot,
        stat.cons_energy_rel,stat.cons_energy_tot,stat.prod_waste_rel,stat.prod_waste_tot};

    for (long unsigned int i = 0; i < (sizeof(col1)/sizeof(col1[0]))-1; i++) {
        printf("%-*s | %-*d\n", col_width, col1[i], col_width, col2[i]);
    }

    printf("%d\n", count++);
    
}

int stat_total_value(int * tot_value, int * rel_value) {
        *tot_value += *rel_value;
        return *tot_value;
    }