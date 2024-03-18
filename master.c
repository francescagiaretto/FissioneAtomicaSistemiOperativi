#include "library.h"

void set_sem_values();
int shmid, semid;
data_buffer * shmem_ptr;
pid_t pid_alimentatore, pid_attivatore;
pid_t * pid_atoms;

void signal_handler(int sig) {
  switch(sig) {
    case SIGALRM:
      shmem_ptr -> message = "timeout.";
      raise(SIGUSR1);
    break;

    case SIGINT:
     raise(SIGUSR1);
    break;

    case SIGTERM:
      raise(SIGUSR1);
    break;

    case SIGUSR1:
      printf("Simulation terminated due to %s\n", shmem_ptr -> message);
      // kill(pid_alimentatore, SIGTERM);
      // kill(pid_attivatore, SIGTERM);
      shmdt(shmem_ptr);
      shmctl(shmid, IPC_RMID, NULL);
      semctl(semid, 0, IPC_RMID);
      //* come dico a tutti i processi atomo di fermarsi?
      exit(0);
    break;
  }
}

int main(int argc, char* argv[]) {

  int atomic_num;
  pid_t pid_alimentatore, pid_attivatore;
  pid_t * pid_atoms;
  key_t shmkey, semkey;
  char n_atom[8], id_shmat[8], pointer_shmem[8], id_sem[8];

  shmkey = ftok("master.c", 'A');
  semkey = ftok("master.c", 'B');
  shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
  TEST_ERROR;

    // creating semaphore to handle the simulation
  semid = semget(semkey, 3, IPC_CREAT | 0666);
  TEST_ERROR;

  shmem_ptr = (data_buffer *)shmat(shmid, NULL, 0); // NULL for improved code portability: address may not be available outside of Unix
  TEST_ERROR;

  set_sem_values();

  sem.sem_flg = 0;

  sprintf(id_shmat, "%d", shmid);
  char * vec_alim[] = {"alimentatore", id_shmat, id_sem, NULL};
  sprintf(id_sem, "%d", semid);
  char * vec_attiv[] = {"attivatore", id_sem, NULL};

  switch(pid_alimentatore = fork()) {
    case -1:
      shmem_ptr -> message = "meltdown.";
      raise(SIGUSR1);
    break;

    case 0:
      sem.sem_num = WAITSEM;
      sem.sem_op = 1;
      semop(semid, &sem, 1);
      CHECK_OPERATION;

      execve("./alimentatore", vec_alim, NULL);
      TEST_ERROR;
    break;

    default: // parent process
      switch(pid_attivatore = fork()) {

          case -1:
            shmem_ptr -> message = "meltdown.";
            raise(SIGUSR1);
          break;

          case 0:
            sem.sem_num = WAITSEM;
            sem.sem_op = 1;
            semop(semid, &sem, 1);
            CHECK_OPERATION;

            execve("./attivatore", vec_attiv, NULL);
            TEST_ERROR;
          break;
      }
    break;
  } 

  pid_atoms = malloc(sizeof(pid_t) * N_ATOM_INIT);    // dynamic mem allocated for pid atomi array

  // creating children
  for(int i = 0; i < N_ATOM_INIT; i++) {

    pid_atoms[i] = fork();

    // generating random pid for children
    srand(getpid());
    atomic_num = rand() % N_ATOM_MAX + 1;
    sprintf(n_atom, "%d", atomic_num);
    char * vec_atomo[] = {"atomo", n_atom, id_shmat, id_sem, NULL};

    switch(pid_atoms[i]) {

      case -1:
        shmem_ptr -> message = "meltdown.";
        raise(SIGUSR1);
      break;

      case 0: 
        free(pid_atoms);
        sem.sem_num = WAITSEM;
        sem.sem_op = 1;
        semop(semid, &sem, 1);
        CHECK_OPERATION;

        execve("./atomo", vec_atomo, NULL);
        TEST_ERROR;
      break;

      default:
      break;
    }
  }


  // ! once everything is set the simulation starts (lasting SIM_DURATION seconds)
  struct sigaction sa;

  bzero(&sa, sizeof(&sa)); // emptying struct to send to child
  sa.sa_handler = &signal_handler;
  sigaction(SIGALRM, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);

  free(pid_atoms);

  printf("ATTENDO I MIEI FIGLI....\n");
  sem.sem_num = WAITSEM;
	sem.sem_op = -(N_ATOM_INIT + 2);
  semop(semid, &sem, 1);
  CHECK_OPERATION;

  printf("PRE SIMULAZIONE\n");
  sleep(5);
  alarm(SIM_DURATION);
  
  sem.sem_num = STARTSEM;
  sem.sem_op = N_ATOM_INIT +2;
  semop(semid, &sem, 1);
  CHECK_OPERATION;
  
  printf("HO COMINCIATO LA SIMULAZIONE\n");

  // !!! cambiare condizione for 
  for(; 1; ) {
    sleep(1);
    // checking explode condition
    if (shmem_ptr -> prod_en_tot  >= ENERGY_EXPLODE_THRESHOLD) {
      shmem_ptr -> message = "explode.";
      raise(SIGUSR1);
    }

    // blackout
  /*  if (ENERGY_DEMAND > shmem_ptr -> prod_en_tot) {
      message = "blackout.";
      signal(SIGUSR1, term_handler);
    } else {
      available_en = shmem_ptr -> prod_en_tot - ENERGY_DEMAND;
    } */
    
    print_stats(shmem_ptr);

  }
}

void set_sem_values(){
  
  //prevents the simulation from starting before everything is set
  semctl(semid, WAITSEM, SETVAL, 0);
  TEST_ERROR;

  //allows the simulation to start after everything is set
  semctl(semid, STARTSEM, SETVAL, 0);
  TEST_ERROR;

  // handles waste
  semctl(semid, WASTESEM, SETVAL, 1);
  TEST_ERROR;
}