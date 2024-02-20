#include "library.h"

int main(int argc, char* argv[]){

    int n_atomico_padre = atoi(argv[1]); int EN_LIB = 0;

    if (n_atomico_padre <= MIN_N_ATOMICO) { // controllo se il pid è inferiore al numero atomico minimo
        // termino processo
        // scorie++  

        //! CODA DI MESSAGGI IDEA PER EN_LI DA PASSARE       
    }  

    /*
    1. Crei la coda di messaggi id = msggget(key_ t chiave, IPC_CREAT, 0600) IPC creat accede sempre alla coda di messaggi o la crea
    2. if (coda == -1) {

        ErrExit("msgget")

    }
    3. TODO ricorda di fare la cancellazione immediata appena usate
    4. il valore di ritorno è una struct
    5. crei un errore apposito per energia liberata < 0
    */

    int nipote1 = 0; int nipote2= 0; int vec_nipoti[] = {nipote1,nipote2};

    char appoggio[3];

    nipote1 = rand()% n_atomico_padre + 1;
    nipote2 = n_atomico_padre - nipote1;


    // TODO gestire la fork quando lo richiede l'attivatore.
    for (int i = 0 ; i < 2; i++) {

        pid_t pid_atomi = fork();
        
        srand(getpid()); //*  meglio usare getpid piuttosto che time(NULL) perché time randomizza in base al tempo del programma
                        //*  mentre così usa il pid che è sempre diverso, il tempo del programma invece è sempre lo stesso

        sprintf(appoggio, "%d",vec_nipoti[i]);
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
                int max_num = MAX(nipote1, nipote2);
                EN_LIB = nipote1*nipote2 - max_num; // NECESSARIO FARLO ANCHE NEL NIPOTE?         
                printf("Padre: %d, Nipote1: %d, Nipote2: %d\n", n_atomico_padre, nipote1, nipote2);
                break;
            }
    }
}