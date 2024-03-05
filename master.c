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
        perror("Shared memory creation.\n"); exit(EXIT_FAILURE);
    }

    sprintf(id_shmat, "%d", shmid);

    // access to shmem is handled through attached pointer
    shmem_p = (data_buffer *)shmat(shmid, NULL, 0); // NULL for improved code portability: address may not be available
                                                    // outside of Unix
    if (shmem_p == (void *) -1) {
        perror("Pointer not attached.\n"); exit(EXIT_FAILURE);
    }

    char * vec_alim[] = {"alimentatore", id_shmat, NULL};
    char * vec_attiv[] = {"attivatore", NULL};


    // creating attivatore and alimentatore
    switch(pid_alimentatore = fork()) {
        case -1:
            printf("Simulation terminated due to meltdown.\n"); exit(EXIT_FAILURE);
        break;

        case 0:
            if(execve("./alimentatore", vec_alim, NULL) == -1) {perror("Execve alim"); exit(EXIT_FAILURE);}
        break;

        default: // parent process
            switch(pid_attivatore = fork()) {

                case -1:
                    printf("Simulation terminated due to meltdown.\n");
                    exit(EXIT_FAILURE);
                break;

                case 0:
                    if(execve("./attivatore", vec_attiv, NULL) == -1) { perror("Execve attiv"); exit(EXIT_FAILURE);} 
                break;
            }
        break;
    }

    pid_atoms = malloc(sizeof(pid_t) * N_ATOM_INIT);    // dynamic mem allocated for pid atomi array

    // creating children
    for(int i = 0; i < N_ATOM_INIT; i++) {
        pid_atoms[i] = fork();

        // generating random pid for children
        srand(getpid());
        atomic_num = rand() % RNG_N_ATOMICO + 1;
        sprintf(n_atom, "%d", atomic_num);
        char * vec_atomo[] = {"atomo", n_atom, id_shmat, NULL};

        switch(pid_atoms[i]) {

            case -1:
                printf("Simulation terminated due to meltdown.\n");
                exit(EXIT_FAILURE);
            break;

            case 0: // children: freeing allocated memory
                free(pid_atoms);
                if (execve("./atomo", vec_atomo, NULL) == -1) {perror("Execve atomo"); exit(EXIT_FAILURE);}
            break;

            default:
                printf("Atom's pid is %d\n", pid_atoms[i]);
            break;
        }
    }

    // ! once everything is set the simulation starts (lasting SIM_DURATION seconds)
    struct sigaction sa;

    bzero(&sa, sizeof(&sa)); // emptying struct to send to child
    sa.sa_handler = &signal_handler;
    sigaction(SIGALRM, &sa, NULL);

    free(pid_atoms);
    alarm(SIM_DURATION);

    //? vogliamo metterla in shmem?
    //int available_en;
        
    for(; 1; ) {
        
        relative.prod_waste_rel = shmem_p -> data[0];
        relative.prod_energy_rel = shmem_p -> data[1];

        print_stats(relative, total);

      /*  if (ENERGY_DEMAND > total.prod_energy_tot) {
            printf("Simulation terminated due to blackout.\n"); exit(EXIT_FAILURE);
        } else {
            available_en = total.prod_energy_tot - ENERGY_DEMAND;
        } */
        
        sleep(1);
        bzero(&relative, sizeof(&relative));
    }
}


void print_stats(stat_rel relative, stat_tot total) {
    static int count = 0;

    int col1_width = 35;
    int col2_width = 10;
    printf("\n\n\n\n");
    printf("STATS:\n");
    for (int i = 0; i <= (col1_width + col2_width); i++) {
        printf("-");
    }
    printf("\n");

    char *col1[11] = {"Last second activations:","Total activations:","Last second divisions:","Total divisions:",
        "Last second produced energy:","Total produced energy:","Last second consumed energy:","Total consumed energy:",
        "Last second waste:","Total waste:"};
    int col2[11] = {relative.n_activ_rel,total.n_activ_total,relative.n_div_rel,total.n_div_total,relative.prod_energy_rel,total.prod_energy_tot,
        relative.cons_energy_rel,total.cons_energy_tot,relative.prod_waste_rel,total.prod_waste_tot};

    for (long unsigned int i = 0; i < (sizeof(col1)/sizeof(col1[0]))-1; i++) {
        printf("%-*s | %-*d\n", col1_width, col1[i], col2_width, col2[i]);
        if (i%2!=0) {
            for (int i = 0; i <= (col1_width + col2_width); i++) {
                printf("-");
            }
            printf("\n");
        }
    }

    printf("Simulation timer: %d\n", count++);
}

// ! metodo non usato ancora, da aggiustare per gestire i valori totali
int stat_total_value(int * tot_value, int * rel_value) {
        *tot_value += *rel_value;
        return *tot_value;
} // ! crea il ricevitore di dato e prova a stampare