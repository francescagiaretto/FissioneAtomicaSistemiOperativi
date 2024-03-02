#include "library.h"

void generate_n_atom(int *, int *);

int main(int argc, char* argv[]){
    
    int n_atomico_padre = atoi(argv[1]); int n_atomico_figlio, en_lib, shmid;
    buffer_dati * shmem_p;
    char n_atom_scissione[3];
    
    // il figlio si collega alla shmem
    shmid = atoi(argv[2]);
    shmem_p = (buffer_dati *) shmat(shmid, NULL, 0);
    if (shmem_p == (void *) -1) {
        perror("Pointer not attached."); exit(EXIT_FAILURE);
    }

    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        shmem_p -> data[0] = shmem_p -> data[0] + 1;
        kill(getpid(), SIGTERM);
    } 

    srand(getpid()); //*  meglio usare getpid piuttosto che time(NULL) perché time randomizza in base al tempo del programma
                    //*  mentre così usa il pid che è sempre diverso, il tempo del programma invece è sempre lo stesso

    generate_n_atom(&n_atomico_padre, &n_atomico_figlio);

    // TODO gestire la fork quando lo richiede l'attivatore.

    sprintf(n_atom_scissione, "%d", n_atomico_figlio);
    char * vec_atomo[] = {"atomo", n_atom_scissione, NULL};

    // TODO funzione energy() che incrementa l'energia liberata nelle statistiche del master
        switch (fork())
        {
            case -1: // meltdown
                printf("Simulazione terminata: meltdown.");
                exit(EXIT_FAILURE);
            break;
            
            case 0: // controlli il figlio
                shmdt(shmem_p);
                if(execve("atomo", vec_atomo, NULL)==-1) {perror("Execve nipote"); exit(EXIT_FAILURE);} 
            break;
            
            default: // controlli il padre
                en_lib = n_atomico_figlio*n_atomico_padre - MAX(n_atomico_figlio, n_atomico_padre);    
                // TODO inviare energia liberata al master
                printf("Padre: %d, n_atomico_figlio: %d\n", n_atomico_padre, n_atomico_figlio);
            break;
        }
}

// metodo che calcola il numero atomico dopo la scissione
void generate_n_atom(int * n_atomico_padre, int * n_atomico_figlio) {
    int temp = *n_atomico_padre;
    *n_atomico_padre = rand()% *n_atomico_padre + 1;
    *n_atomico_figlio = temp - *n_atomico_padre;

}