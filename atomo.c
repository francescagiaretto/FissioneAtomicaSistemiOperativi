#include "library.h"

int main(int argc, char* argv[]){
    if (getpid() == -1) { // controllo iniziale: se l'atomo non ha pid meltdown
        printf("meltdown.\n");
        exit(EXIT_FAILURE);
    } else if (getpid() <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        // termino processo
        // scorie++
    }

    // scissione atomica: pid padre + pid figlio = pid originale padre
}