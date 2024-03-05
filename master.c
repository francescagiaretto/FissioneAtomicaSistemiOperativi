#include "library.h"

void print_stats(stat_rel, stat_tot);
int stat_total_value(int *, int *);

int shmid;
data_buffer * shmem_p;

void signal_handler(int sig) {  // handles sigalarm from alarm(n), which defines the "timeout" condition
    write(0, "Simulation terminated due to timeout.\n", 38);
    shmdt(shmem_p);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char* argv[]) {

    int atomic_num;
    pid_t pid_alimentatore, pid_attivatore;
    pid_t * pid_atoms;
    key_t shmkey;

    // setting structs to 0
    stat_tot total = {0};
    stat_rel relative = {0};
    char n_atom[4]; char id_shmat[4];

    // checking explode condition
    if (total.cons_energy_tot >= ENERGY_EXPLODE_THRESHOLD) {
        printf("Simulation terminated due to explosion.\n"); exit(EXIT_FAILURE);
    }

    // generating shared memory key
    shmkey = ftok("master.c", 'A');
    // creating shared memory
    shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Shared memory creation"); exit(EXIT_FAILURE);
    }

    // collego alla memoria una variabile puntatore per l'accesso alla shmem
    data_buffer * shmem_p;
    shmem_p = (data_buffer *)shmat(shmid, NULL, 0); // NULL perché un altro indirizzo riduce la portabilità del codice: un 
                                            // indirizzo valido in Unix non è per forza valido altrove
    if (shmem_p == (void *) -1) {
        perror("Pointer not attached"); exit(EXIT_FAILURE);
    }


    // creazione processo attivatore e alimentatore
    switch(pid_alimentatore = fork()) {
        // controlla se c'è meltdown

        case -1:
            printf("Simulazione terminata: meltdown."); exit(EXIT_FAILURE);
        break;

        case 0:
            if(execve("./alimentatore", vec_alim , NULL) == -1) {perror("Execve alim"); exit(EXIT_FAILURE);}
        break;

        default: // siamo nel processo padre
            switch(pid_attivatore = fork()) {

                case -1:
                    printf("Simulazione terminata: meltdown.");
                    exit(EXIT_FAILURE);
                break;

                case 0:
                    if(execve("./attivatore", vec_attiv, NULL) == -1) { perror("Execve attiv"); exit(EXIT_FAILURE);} 
                break;
            }
        break;
    }

    pid_atomi = malloc(sizeof(pid_t) * N_ATOM_INIT);    // allocazione dinamica di memoria per l'array di pid atomi

    // creazione processi atomo
    for(int i = 0; i < N_ATOM_INIT; i++) {
        pid_atomi[i] = fork();

        int range = 118; // numero atomico compreso tra 1 e 118 (max tavola periodica)

        srand(getpid());
        int num_atomico = rand() % range + 1;
        sprintf(n_atom, "%d", num_atomico);
        sprintf(id_shmat, "%d", shmid);
        char * vec_atomo[] = {"atomo", n_atom, id_shmat, NULL};

        switch(pid_atomi[i]) {

            case -1:
                printf("Simulazione terminata: meltdown.");
                exit(EXIT_FAILURE);
            break;

            case 0: // caso figli: libero la memoria allocata con la malloc
                free(pid_atomi);
                if (execve("./atomo", vec_atomo, NULL) == -1) {perror("Execve atomi"); exit(EXIT_FAILURE);}  // non funziona
            break;

            default:
                printf("Atomo con pid %d\n", pid_atomi[i]);
            break;
        }
    }

    // ! solo quando tutto è in regola posso far partire la simulazione, che dura SIM_DURATION secondi 
    struct sigaction sa;

    bzero(&sa, sizeof(&sa)); // azzera tutta la struct per passarla al figlio
    sa.sa_handler = &signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    free(pid_atomi);
    alarm(SIM_DURATION);

    printf("%d", shmem_p-> data[0]);
        
    for(; 1; ) {
        
        relative.prod_waste_rel = shmem_p -> data[0];

        print_stats(relative, totali);

        // TODO: prelevare la quantità ENERGY_DEMAND di energia
        
        sleep(1);
        stat_rel relative = {0};
    }

    //memcpy(shmem_p, &relative.prod_waste_rel, sizeof(shmem_p));

    shmdt(shmem_p);
    shmctl(shmid, IPC_RMID, NULL);
}


void print_stats(stat_rel relative, stat_tot totali) {
    static int count = 0;

    int col_width = 40;
    printf("\n\n\n\n");
    printf("STATISTICHE:\n");
    printf("%-*s | %-*s\n", col_width, "Tipo", col_width, "Valore");
    printf("%-*c | %-*c\n", col_width, '-', col_width, '-');

    char *col1[11] = {"Attivazioni ultimo secondo:","Attivazioni totali:","Scissioni ultimo secondo:","Scissioni totali:",
        "Energia prodotta ultimo secondo:","Energia prodotta totale:","Energia consumata ultimo secondo:","Energia consumata totale:",
        "Scorie prodotte ultimo secondo:","Scorie prodotte totali:"};
    int col2[11] = {relative.n_activ_rel,totali.n_activ_total,relative.n_div_rel,totali.n_div_total,relative.prod_energy_rel,totali.prod_energy_tot,
        relative.cons_energy_rel,totali.cons_energy_tot,relative.prod_waste_rel,totali.prod_waste_tot};

    for (long unsigned int i = 0; i < (sizeof(col1)/sizeof(col1[0]))-1; i++) {
        printf("%-*s | %-*d\n", col_width, col1[i], col_width, col2[i]);
    }

    printf("%d\n", count++);
}

// ! metodo non usato ancora, da aggiustare per gestire i valori totali
int stat_total_value(int * tot_value, int * rel_value) {
        *tot_value += *rel_value;
        return *tot_value;
    }



// ! crea il ricevitore di dato e prova a stampare