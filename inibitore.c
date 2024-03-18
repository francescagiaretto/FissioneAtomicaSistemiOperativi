#include "library.h"

int main(char* argv[]) {

    // bisogna poter decidere a runtime se si vuole che l'inibitore sia attivo o meno 
      // (l'utente deve poter fermare e far ripartire il processo inibitore più volte da terminale)

    int inib_on = argv[1]; // values 0 and 1; 0 means off and 1 means on

    while (inib_on == 1) {
      // assorbe parte dell'energia prodotta dalla scissione dell'atomo diminuendo la quantità di energia liberata

      // limita il numero di scissioni agendo sull'operazione di scissione rendendola probabilistica (decidendo se 
        // la scissione debba avvenire o meno oppure trasformando in  scoria uno degli atomi prodotti dopo la scissione)
      
      // non devono avvenire explode e meltdown
    }
}