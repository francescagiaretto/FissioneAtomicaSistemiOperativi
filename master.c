#include "library.h"

void set_sem_values();
int shmid, semid, inib_on, available_en;
data_buffer * shmem_ptr;
pid_t pid_alimentazione, pid_attivatore;


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
      shmem_ptr -> termination = 1;
      int status = 0;
      waitpid(-1, &status, WIFEXITED(status));
      printf("Simulation terminated due to %s\n", shmem_ptr -> message);

      kill(pid_alimentazione, SIGTERM);
      kill(pid_attivatore, SIGTERM);
      shmdt(shmem_ptr);
      shmctl(shmid, IPC_RMID, NULL);
      semctl(semid, 0, IPC_RMID);
      exit(0);
    break;

    case SIGUSR2:
      if(inib_on == 0) {
        inib_on = 1;
        printf("Inibitore attivato. Puoi disattivarlo quando vuoi.\n");
      } else if (inib_on == 1) {
        inib_on = 0;
        printf("Inibitore disattivato. Puoi attivarlo quando vuoi.\n");
      }
    break;

    case SIGCHLD:
      shmem_ptr -> attiv_signal = 1;
    break;
  }
}

int main(int argc, char* argv[]) {

  int atomic_num;
  pid_t * pid_atoms;
  long unsigned shmkey, semkey;
  char n_atom[8], id_shmat[8], pointer_shmem[8], id_sem[8];
  srand(getpid());

  shmkey = ftok("master.c", 'A');
  semkey = ftok("master.c", 'B');
  shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
  TEST_ERROR;

    // creating semaphore to handle the simulation
  semid = semget(semkey, 6, IPC_CREAT | 0666);
  TEST_ERROR;

  printf("MASTER: %d, shmid: %d, semid: %d\n\n", getpid(), shmid, semid);

  shmem_ptr = (data_buffer *)shmat(shmid, NULL, 0); // NULL for improved code portability: address may not be available outside of Unix
  TEST_ERROR;

  set_sem_values();

  sem.sem_flg = 0;

  sprintf(id_shmat, "%d", shmid);
  char * vec_alim[] = {"alimentazione", id_shmat, id_sem, NULL};
  sprintf(id_sem, "%d", semid);
  char * vec_attiv[] = {"attivatore", id_sem, id_shmat, NULL};

  switch(pid_alimentazione = fork()) {
    case -1:
      shmem_ptr -> message = "meltdown.";
      raise(SIGUSR1);
    break;

    case 0:
      sem.sem_num = WAITSEM;
      sem.sem_op = 1;
      semop(semid, &sem, 1);
      CHECK_OPERATION;

      execve("./alimentazione", vec_alim, NULL);
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
  alarm(SIM_DURATION);
  
  sem.sem_num = STARTSEM;
  sem.sem_op = N_ATOM_INIT +2;
  semop(semid, &sem, 1);
  CHECK_OPERATION;

  shmem_ptr -> cons_en_rel = ENERGY_DEMAND;
  
  printf("HO COMINCIATO LA SIMULAZIONE\n");

  // !!! cambiare condizione for 
  for(; 1; ) {
    sleep(1);
    // checking explode condition
    if (shmem_ptr -> prod_en_tot  >= ENERGY_EXPLODE_THRESHOLD) {
      shmem_ptr -> message = "explode.";
      raise(SIGUSR1);
    }
    
    stat_total_value(shmem_ptr);
    print_stats(shmem_ptr);

    //blackout
    if (ENERGY_DEMAND > shmem_ptr -> prod_en_tot) {
      shmem_ptr -> message = "blackout.";
      raise(SIGUSR1);
    } else {
      shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - shmem_ptr -> cons_en_rel;
    }

    /*
      if (inib_on == 0) {
        printf("Vuoi attivare l'inibitore?\n");
        if (risposta == y || risposta == Y || risposta == enter) {
          raise(SIGUSR2);
          char * vec_inib[] = {"inibitore", inib_on, NULL};
          execve("./inibitore", vec_inib, NULL);
        }
      }
      else if (inib_on == 1) {
        printf("Vuoi disattivare l'inibitore?\n");
        if (risposta == y || risposta == Y || risposta == enter) {
          raise(SIGUSR2);
        }
      }
    */

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

  // handles energy production
  semctl(semid, PROD_ENERGYSEM, SETVAL, 1);
  TEST_ERROR;

  // handles divisions
  semctl(semid, DIVISIONSEM, SETVAL, 1);
  TEST_ERROR;

  // handles activations
  semctl(semid, ACTIVATIONSEM, SETVAL, 1);
  TEST_ERROR;
}