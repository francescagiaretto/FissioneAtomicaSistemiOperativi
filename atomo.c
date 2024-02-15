#include "library.h"

int main(int argc, char* argv[]){
    int n_atomico_bro1; int n_atomico_bro2; int temp;
    int n_atomico_padre = atoi(argv[1]);
    printf("%d", n_atomico_padre);
    
    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        // termino processo
        // scorie++
    }

    switch (fork())
    {
        case -1: // meltdown
            printf("meltdown.");
            exit(EXIT_FAILURE);
        break;
        
        case 0: // controlli il nipote
            srand(time(NULL));
            n_atomico_bro1 = rand()% n_atomico_padre + 1;
            n_atomico_bro2 = n_atomico_padre - n_atomico_bro1;
            printf("Padre: %d, Bro1: %d, Bro2: %d", n_atomico_padre, n_atomico_bro1, n_atomico_bro2);
        break;

        default: // controlli il figlio
        break;
    }
}