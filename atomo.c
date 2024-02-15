#include "library.h"

int main(int argc, char* argv[]){
    int n_atomico_bro1 = 0; int n_atomico_bro2 = 0; int vec_figli[] = {n_atomico_bro1, n_atomico_bro2};
    int n_atomico_padre = atoi(argv[1]);
    char appoggio[3];

    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        // termino processo
        // scorie++
    }
    

    for (int i = 0 ; i < 2; i++) {

        pid_t pid_atomi = fork();

        srand(time(NULL));
        n_atomico_bro1 = rand()% n_atomico_padre + 1;
        n_atomico_bro2 = n_atomico_padre - n_atomico_bro1;
            
        sprintf(appoggio, "%d",vec_figli[i]);
        char * vec_atomo[] = {"atomo", appoggio, NULL};

            switch (pid_atomi)

            {
                case -1: // meltdown
                    printf("meltdown.");
                    exit(EXIT_FAILURE);
                break;
                
                case 0: // controlli il figlio
                if (execve("atomo", vec_atomo, NULL)==-1) {perror("execve"); exit(EXIT_FAILURE);} 
                break;

                default: // controlli il padre
                printf("Padre: %d, Bro1: %d, Bro2: %d", n_atomico_padre, n_atomico_bro1, n_atomico_bro2);
                break;
            }
    }
}