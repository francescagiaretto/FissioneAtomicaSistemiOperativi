#include "library.h"

int main(int argc, char* argv[]){
    int nipote1 = 0; int nipote2= 0; int vec_nipoti[] = {nipote1,nipote2};
    int n_atomico_padre = atoi(argv[1]);

    char appoggio[3];

    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        // termino processo
        // scorie++
    }

    srand(getpid()); // meglio usare getpid piuttosto che time(NULL) perché time randomizza in base al tempo del programma
                        // mentre così usa il pid che è sempre diverso, il tempo del programma invece è sempre lo stesso

    for (int i = 0 ; i < 2; i++) {

        pid_t pid_atomi = fork();
        
        nipote1 = rand()% n_atomico_padre + 1;
        nipote2 = n_atomico_padre - nipote1;

        sprintf(appoggio, "%d",vec_nipoti[i]);
        char * vec_atomo[] = {"atomo", appoggio, NULL};

            switch (pid_atomi)

            {
                case -1: // meltdown
                    printf("Simulazione terminata: meltdown.");
                    exit(EXIT_FAILURE);
                break;
                
                case 0: // controlli il figlio
                    if (execve("./atomo", vec_atomo, NULL)==-1) {perror("Execve nipote"); exit(EXIT_FAILURE);} 
                break;

                default: // controlli il padre
                    printf("Padre: %d, Nipote1: %d, Nipote: %d\n", n_atomico_padre, nipote1, nipote2);
                break;
            }
    }
}