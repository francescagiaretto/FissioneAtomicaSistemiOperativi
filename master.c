#include "library.h"

int shmid, semid;
data_buffer * shmem_p;
struct sembuf sem;

void signal_handler(int sig) {
  switch(sig) {
    case SIGALRM:
      shmem_p -> message = "timeout.";
      raise(SIGUSR1);
    break;

    case SIGINT:
     raise(SIGUSR1);
    break;

    case SIGTERM:
      raise(SIGUSR1);
    break;

    case SIGUSR1:
      printf("Simulation terminated due to %s\n", shmem_p -> message);
      fflush(stdout);
      shmdt(shmem_p);
      shmctl(shmid, IPC_RMID, NULL);
      semctl(semid, 0, IPC_RMID);
      exit(0);
    break;
  }
}

int main(int argc, char* argv[]) {

  int atomic_num;
  pid_t pid_alimentatore, pid_attivatore;
  pid_t * pid_atoms;
  key_t shmkey, semkey;

  // setting structs to 0
  char n_atom[8], id_shmat[8], pointer_shmem[8], sem_vec[8];

  // generating shared memory key
  shmkey = ftok("master.c", 'A');
  semkey = ftok("master.c", 'B');
  // creating shared memory
  shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("Shared memory creation in master.\n"); exit(EXIT_FAILURE);
  }

  sprintf(id_shmat, "%d", shmid);

  // access to shmem is handled through attached pointer
  shmem_p = (data_buffer *)shmat(shmid, NULL, 0); // NULL for improved code portability: address may not be available
                                                  // outside of Unix
  if (shmem_p == (void *) -1) {
    perror("Pointer not attached in master"); exit(EXIT_FAILURE);
  }


  sprintf(sem_vec, "%d", semkey);
  // creating semaphore to handle the simulation
  semid = semget(semkey, 2, IPC_CREAT | 0666);
  if(semid == -1) {
    perror("semaphore creation in master"); exit(EXIT_FAILURE);
  }
  

  if(semctl(semid, WAITSEM, SETVAL, 0) == -1) {
    perror("semctl iniziale"); exit(EXIT_FAILURE);
  }
  if(semctl(semid, STARTSEM, SETVAL, 0) == -1) {
    perror("semctl iniziale"); exit(EXIT_FAILURE);
  }

  sem.sem_flg = 0;

  char * vec_alim[] = {"alimentatore", id_shmat, sem_vec, NULL};
  char * vec_attiv[] = {"attivatore", sem_vec, NULL};

  // creating attivatore and alimentatore
  switch(pid_alimentatore = fork()) {
    case -1:
      shmem_p -> message = "meltdown.";
      raise(SIGUSR1);
    break;

    case 0:
      sem.sem_num = WAITSEM;
      sem.sem_op = 1;
      if (semop(semid, &sem, 1)==-1){perror("semop startsem"); exit(EXIT_FAILURE);};

      sem.sem_num = STARTSEM;
      sem.sem_op = -1;
      if (semop(semid, &sem, 1)==-1){perror("semop startsem"); exit(EXIT_FAILURE);}; 
      printf("\nTEST ALIMENTATORE\n");
      if(execve("./alimentatore", vec_alim, NULL) == -1) {perror("Execve alim"); exit(EXIT_FAILURE);}
    break;

    default: // parent process
      switch(pid_attivatore = fork()) {

          case -1:
            shmem_p -> message = "meltdown.";
            raise(SIGUSR1);
          break;

          case 0:
            sem.sem_num = WAITSEM;
            sem.sem_op = 1;
            if (semop(semid, &sem, 1)==-1){perror("semop startsem"); exit(EXIT_FAILURE);};

            sem.sem_num = STARTSEM;
            sem.sem_op = -1;
            if (semop(semid, &sem, 1)==-1){perror("semop startsem"); exit(EXIT_FAILURE);}; 
            printf("\nTEST ATTIVATORE\n");
            if(execve("./attivatore", vec_attiv, NULL) == -1) { perror("Execve attiv"); exit(EXIT_FAILURE);} 
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
    char * vec_atomo[] = {"atomo", n_atom, id_shmat, sem_vec, NULL};

    switch(pid_atoms[i]) {

      case -1:
        shmem_p -> message = "meltdown.";
        raise(SIGUSR1);
      break;

      case 0: // children: freeing allocated memory
        free(pid_atoms);
        sem.sem_num = WAITSEM;
        sem.sem_op = 1;
        if (semop(semid, &sem, 1)==-1){perror("semop startsem"); exit(EXIT_FAILURE);}

        sem.sem_num = STARTSEM;
	      sem.sem_op = -1;
  	    if (semop(semid, &sem, 1) == -1){perror("semop startsem"); exit(EXIT_FAILURE);} 
        printf("\nTEST ATOMO\n");
        if (execve("./atomo", vec_atomo, NULL) == -1) {perror("Execve atomo"); exit(EXIT_FAILURE);}
      break;

      default:
      break;
    }
  }

  printf("PORCODIO PORCAMADONNA\n\n\n\n");
  sem.sem_num = WAITSEM;
	sem.sem_op = -(N_ATOM_INIT + 2);
  if (semop(semid, &sem, 1) ==-1){perror("semop startsem"); exit(EXIT_FAILURE);};

  printf("PORCODIO\n\n\n\n");
  // ! once everything is set the simulation starts (lasting SIM_DURATION seconds)
  struct sigaction sa;

  bzero(&sa, sizeof(&sa)); // emptying struct to send to child
  sa.sa_handler = &signal_handler;
  sigaction(SIGALRM, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);

  free(pid_atoms);

  /* if (int sem_op != N_PROC_INIT) {
    fprintf(stderr, "Wrong number of resources.\n"); exit(EXIT_FAILURE);
  } */

  sleep(5);
  alarm(SIM_DURATION);
  sem.sem_num = STARTSEM;
  sem.sem_op = N_ATOM_INIT +2;
  if (semop(semid, &sem, 1)==-1){perror("semop startsem"); exit(EXIT_FAILURE);};
  printf("PORCODIO 2\n\n\n\n");

  //? vogliamo metterla in shmem?
  //int available_en;
  // !!! cambiare condizione for 
  for(; 1; ) {
    sleep(1);
    // checking explode condition
    if (shmem_p -> prod_en_tot >= ENERGY_EXPLODE_THRESHOLD) {
      shmem_p -> message = "explode.";
      raise(SIGUSR1);
    }

    // blackout
  /*  if (ENERGY_DEMAND > shmem_p -> prod_en_tot) {
      message = "blackout.";
      signal(SIGUSR1, term_handler);
    } else {
      available_en = shmem_p -> prod_en_tot - ENERGY_DEMAND;
    } */
    
    print_stats(shmem_p);

  }
}