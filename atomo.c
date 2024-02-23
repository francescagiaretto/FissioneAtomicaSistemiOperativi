#include "library.h"

int main(int argc, char* argv[]){

    int n_atomico_padre = atoi(argv[1]); int EN_LIB = 0;

    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        kill(getpid(), SIGTERM);
        // scorie++         
    }  

    

    int nipote1 = 0; int nipote2= 0; int vec_nipoti[] = {nipote1,nipote2};

    char appoggio[3];

    nipote1 = rand()% n_atomico_padre + 1;
    nipote2 = n_atomico_padre - nipote1;


    // TODO gestire la fork quando lo richiede l'attivatore.
    pid_t pid_atomi = fork();
    
    srand(getpid()); //*  meglio usare getpid piuttosto che time(NULL) perché time randomizza in base al tempo del programma
                    //*  mentre così usa il pid che è sempre diverso, il tempo del programma invece è sempre lo stesso
    sprintf(appoggio, "%d",vec_nipoti);
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