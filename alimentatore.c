#include "library.h"

void signal_handler(int sig);
int semid, shmid;


int main(int argc, char * argv[]) {

   //! bisogna passargli l'id della shared memory perché altrimenti nel vec_atomo non glielo diamo
  int atomic_num;
  char n_atom[4];
  srand(getpid());

  shmid = atoi(argv[1]);
  semid = atoi(argv[2]);
  data_buffer * shmem_p = (data_buffer *) shmat(shmid, NULL, 0);
  TEST_ERROR;

  //* STRUCT defines a nanosec-based sleep (can't be done with standard sleep())
  struct timespec step_nanosec;
  step_nanosec.tv_sec = 0;           // seconds   
  step_nanosec.tv_nsec = STEP_ALIMENTATORE;   // nanoseconds

  sem.sem_num = STARTSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);
  TEST_ERROR;
  // printf("\n\n\nTEST ALIMENTATORE\n\n\n"); 

  for(; 1; ) {
		
	}

  // TODO ogni STEP_ALIMENTATORE nanosecondi deve creare N_NUOVI_ATOMI
  //??? va bene while(1) o c'è un modo più elegante di scriverlo?
  /* while(1) {

    nanosleep(&step_nanosec, NULL); // ricontrolla bene questo, se arriva un segnale va avanti, metti conttollo che riesca a riportarti ad aspettare del tempo

    for(int i = 0; i < N_NUOVI_ATOMI; i++){
      atomic_num = rand() % N_ATOM_MAX + 1;
      sprintf(n_atom, "%d", atomic_num);
      char * vec_atomo[] = {"atomo", n_atom, argv[1], NULL}; // argv[1] = pointer to shmem

      switch(fork()) {

        case -1:
          shmem_p -> message = "meltdown.";
          kill(getppid(), SIGUSR1);
        break;

        case 0:
          if (execve("atomo", vec_atomo, NULL)==-1) {perror("Execve grandchild"); exit(EXIT_FAILURE);} 
        break;

        default:
        break;
      }
    }
  } */
}

