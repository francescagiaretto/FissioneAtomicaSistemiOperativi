#include "library.h"

int main(int argc, char* argv[]){
    printf("%d", atoi(argv[1]));
    
    if (getpid() <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        // termino processo
        // scorie++
    }

    // scissione atomica: pid padre + pid figlio = pid originale padre
}