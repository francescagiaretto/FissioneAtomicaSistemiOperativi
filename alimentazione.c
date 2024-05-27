#include "library.h"

void signal_handler(int sig);
int semid, shmid, msgid;
data_buffer * shmem_ptr;

void signal_handler(int sig) {
	switch(sig) {
		case SIGQUIT:

		break;

    case SIGSEGV:
      kill(shmem_ptr -> pid_master, SIGSEGV);
    break;
	}
}

int main(int argc, char * argv[]) {

  int atomic_num;
  char n_atom[4];
  srand(getpid());

  shmid = atoi(argv[1]);
  semid = atoi(argv[2]);
  msgid = atoi(argv[3]);

  shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
  TEST_ERROR;

  //* STRUCT defines a nanosec-based sleep (can't be done with standard sleep())
  struct timespec step_nanosec;
  step_nanosec.tv_sec = 0;           // seconds   
  step_nanosec.tv_nsec = STEP_ALIMENTAZIONE;   // nanoseconds

  struct sigaction sa;
  bzero(&sa, sizeof(sa)); // emptying struct to send to child
  sa.sa_handler = &signal_handler;
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);

  sem.sem_num = STARTSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);
  
  while(shmem_ptr -> termination != 1) {
    nanosleep(&step_nanosec, NULL); // ricontrolla bene questo, se arriva un segnale va avanti, metti conttollo che riesca a riportarti ad aspettare del tempo
    
    for(int i = 0; i < N_NUOVI_ATOMI; i++){
      atomic_num = rand() % N_ATOM_MAX + 1;
      sprintf(n_atom, "%d", atomic_num);
      char * vec_atomo[] = {"./atomo", n_atom, argv[1], argv[2], argv[3], NULL}; // argv[1] = pointer to shmem
    
      switch(fork()) {

        case -1:
          sem.sem_num = MELTDOWNSEM;
          sem.sem_op = -1;
          semop(semid, &sem, 1);
          kill(shmem_ptr -> pid_master, SIGUSR2);
        break;

        case 0:
          execve("./atomo", vec_atomo, NULL);
          TEST_ERROR;
        break;
      } 
    } 
  } 
}

