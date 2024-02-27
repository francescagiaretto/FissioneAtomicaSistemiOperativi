#include "library.h"

int main(int argc, char* argv[]){
    // necessario crearti vettore per muoverti in memoria condivisa
    
    int n_atomico_padre = atoi(argv[1]); 
    buffer_dati * shmem_p;
    int figlio = 0, vec_nipoti[] = {figlio}, EN_LIB = 0;
    
    // il figlio si collega alla shmem
    int shmid = atoi(argv[2]);
    shmem_p = (buffer_dati *) shmat(shmid, NULL, 0);
    if (shmem_p == (void *) -1) {
        perror("Pointer not attached."); exit(EXIT_FAILURE);
    }
    
    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        shmem_p -> num_scorie = shmem_p -> num_scorie + 1;
        //memcpy(shmem_p -> num_scorie, &scorie, sizeof(scorie)); // destinazione, origine, numero di byte scritti
        kill(getpid(), SIGTERM);
    } 

    char appoggio[3];

    int temp = n_atomico_padre;
    n_atomico_padre = rand()% n_atomico_padre + 1;
    figlio = temp - n_atomico_padre;

    // TODO gestire la fork quando lo richiede l'attivatore.
    pid_t pid_atomi = fork();
    
    srand(getpid()); //*  meglio usare getpid piuttosto che time(NULL) perché time randomizza in base al tempo del programma
                    //*  mentre così usa il pid che è sempre diverso, il tempo del programma invece è sempre lo stesso
    sprintf(appoggio, "%d", *vec_nipoti);
    char * vec_atomo[] = {"atomo", appoggio, NULL};

    // TODO funzione energy() che incrementa l'energia liberata nelle statistiche del master
        switch (pid_atomi)
        {
            case -1: // meltdown
                printf("Simulazione terminata: meltdown.");
                exit(EXIT_FAILURE);
            break;
            
            case 0: // controlli il figlio
                if(execve("atomo", vec_atomo, NULL)==-1) {perror("Execve nipote"); exit(EXIT_FAILURE);} 
            break;
            
            default: // controlli il padre
                int max_num = MAX(figlio, n_atomico_padre);
                EN_LIB = figlio*n_atomico_padre - max_num; // NECESSARIO FARLO ANCHE NEL NIPOTE?         
                printf("Padre: %d, Figlio: %d\n", n_atomico_padre, figlio);
            break;
        }
}