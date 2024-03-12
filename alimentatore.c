#include "library.h"

void signal_handler(int sig);
int semid;


int main(int argc, char * argv[]) {
  semid = semget(atoi(argv[2]), 1, IPC_CREAT | 0666);
  semctl(semid, 0, SETVAL, 1);

  sem.sem_num = WAITSEM;
  sem.sem_op = 0;
  semop(semid, &sem, 1);
  printf("\n\n\nTEST ALIMENTATORE\n\n\n");

  //! bisogna passargli l'id della shared memory perché altrimenti nel vec_atomo non glielo diamo
  int atomic_num, shmid;
  int key= atoi(argv[1]);
  char n_atom[4];
  srand(getpid());

  shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("Shared memory creation.\n"); exit(EXIT_FAILURE);
  }

  data_buffer * shmem_p = (data_buffer *) shmat(key, NULL, 0);
  if (shmem_p == (void *) -1) {
    perror("Pointer not attached."); exit(EXIT_FAILURE);
  }

  //* STRUCT defines a nanosec-based sleep (can't be done with standard sleep())
  struct timespec step_nanosec;
  step_nanosec.tv_sec = 0;           // seconds   
  step_nanosec.tv_nsec = STEP_ALIMENTATORE;   // nanoseconds


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

