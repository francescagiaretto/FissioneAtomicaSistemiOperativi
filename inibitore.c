#include "library.h"

int semid, shmid, msgid;
data_buffer * shmem_ptr;

void signal_handler(int sig){
  switch (sig) {
    case SIGQUIT:
      if (shmem_ptr -> inib_on != 0) {
        write(0, "Inibitore OFF. Turn off anytime with ctrl + backslash \n", 55);

        shmem_ptr -> inib_on = 0;
        
      } else if (shmem_ptr -> inib_on == 0) {
        
        kill(shmem_ptr -> pid_master, SIGUSR1);
      }
    break;

    case SIGSEGV:
      kill(shmem_ptr -> pid_master, SIGSEGV);
    break;
  }
}

int main(int argc, char* argv[]) {

  int absorbing_energy;

  semid = atoi(argv[1]);
	shmid = atoi(argv[2]);
	msgid = atoi(argv[3]);

  shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

  srand(getpid());

  struct sigaction sa;
  bzero(&sa, sizeof(sa)); 
  sa.sa_handler = &signal_handler;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);

  struct timespec step_nanosec;
  step_nanosec.tv_sec = 0;           // seconds   
  step_nanosec.tv_nsec = STEP_INIBITORE;   // nanoseconds

  shmem_ptr -> inib_on = 0;

  sem.sem_num = STARTSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);

  printf("inibit\n");

  sem.sem_num = INIBSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);

  while (shmem_ptr -> termination == 0) {
    nanosleep(&step_nanosec, NULL);
    
    //! se l'inibitore viene disattivato il semaforo blocca la stampa dell'energia evitando l'attesa attiva
    if(shmem_ptr -> inib_on == 0) {
      sem.sem_num = INIBSEM;
      sem.sem_op = -1;
      semop(semid, &sem, 1);
    }
    
    //! assorbe un quinto dell'energia totale
    absorbing_energy = shmem_ptr -> prod_en_rel/10;
    shmem_ptr -> prod_en_rel = shmem_ptr -> prod_en_rel - absorbing_energy;
    shmem_ptr -> absorbed_en_rel = shmem_ptr -> absorbed_en_rel + absorbing_energy;
  }
}