#include "library.h"

int main(int argc, char* argv[]){
    
    printf("%l", strtol(argv[1], sizeof(argv[1]), 10));
    
    // controllo iniziale: se l'atomo non ha pid meltdown
    // scissione atomica: pid padre + pid figlio = pid originale padre
}