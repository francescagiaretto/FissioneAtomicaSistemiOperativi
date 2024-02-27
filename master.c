    #include "library.h"

// ? come giostrare i metodi nella libreria ?

void print_stats(stat_rel, stat_tot);
int stat_total_value(int *, int *);

void signal_handler(int sig) {  // gestisce il segnale di sigalarm che arriva dall'alarm(30), che definisce la terminazione "timeout"
    write(0, "Simulazione terminata: timeout.\n", 32);
    exit(0);
}

int main(int argc, char* argv[]) {

    // TODO controllare energy_explode_threshold

    pid_t pid_alimentatore, pid_attivatore;     // pid dei processi alimentatore e attivatore
    pid_t * pid_atomi;

    // inizializzo le struct a 0
    stat_tot totali = {0}; stat_rel relative = {0};
    char n_atom[4]; 

    char * vec_alim[] = {"alimentatore", NULL};
    char * vec_attiv[] = {"attivatore", NULL};

    // creo la chiave della shared memory
    key_t shmkey = ftok("master.c", 'A');
    // creo la memoria condivisa
    int shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT|0666);
    if (shmid == -1) {
        perror("Shared memory creation"); exit(EXIT_FAILURE);
    }

    // collego alla memoria una variabile puntatore per l'accesso alla shmem
    buffer_dati * shmem_p;
    shmem_p = (buffer_dati *)shmat(shmid, NULL, 0); // NULL perché un altro indirizzo riduce la portabilità del codice: un 
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
            if(execve("./alimentatore", vec_alim, NULL) == -1) {perror("Execve alim"); exit(EXIT_FAILURE);}
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

        srand(getpid());
        int num_atomico = rand() % RNG_N_ATOMICO + 1;
        sprintf(n_atom, "%d", num_atomico);
        char * vec_atomo[] = {"atomo", n_atom, (char*)shmem_p, NULL};

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
    

    for(; 1; ) {
        int scorie = shmem_p -> num_scorie;
        //memcpy(&relative.prod_waste_rel, &scorie, sizeof(scorie)); //! CONTROLLA MEGLIO
        // relative.prod_waste_rel = shmem_p -> num_scorie;
        // shmem_p -> num_scorie;
        printf("%d\n", scorie);
        print_stats(relative, totali);

        // TODO: prelevare la quantità ENERGY_DEMAND di energia
        
        sleep(1);
        stat_rel relative = {0};
    }


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