#include "library.h"

void print_stats(data_buffer * shmem_p);
void  stat_total_value();

int shmid;
data_buffer * shmem_p;

void signal_handler(int sig) {
  switch(sig) {
    case SIGALRM:
      shmem_p -> message = "timeout.";
      raise(SIGUSR1);
    break;

    case SIGTERM:
      exit(0);
    break;

    case SIGUSR1:
      printf("Simulation terminated due to %s\n", shmem_p -> message);
      fflush(stdout);
      shmdt(&shmem_p);
      shmctl(shmid, IPC_RMID, NULL);
      exit(0);
    break;
  }
}

int main(int argc, char* argv[]) {

  int atomic_num;
  pid_t pid_alimentatore, pid_attivatore;
  pid_t * pid_atoms;
  key_t shmkey;

  // setting structs to 0
  char n_atom[4], id_shmat[4], pointer_shmem[8];

  // generating shared memory key
  shmkey = ftok("master.c", 'A');
  // creating shared memory
  shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("Shared memory creation.\n"); exit(EXIT_FAILURE);
  }

  sprintf(id_shmat, "%d", shmid);

  // access to shmem is handled through attached pointer
  shmem_p = (data_buffer *)shmat(shmid, NULL, 0); // NULL for improved code portability: address may not be available
                                                  // outside of Unix
  if (shmem_p == (void *) -1) {
    perror("Pointer not attached.\n"); exit(EXIT_FAILURE);
  }

  char * vec_alim[] = {"alimentatore", id_shmat, NULL};
  char * vec_attiv[] = {"attivatore", NULL};

  // creating attivatore and alimentatore
  switch(pid_alimentatore = fork()) {
    case -1:
      shmem_p -> message = "meltdown.";
      raise(SIGUSR1);
    break;

    case 0:
      if(execve("./alimentatore", vec_alim, NULL) == -1) {perror("Execve alim"); exit(EXIT_FAILURE);}
    break;

    default: // parent process
      switch(pid_attivatore = fork()) {

          case -1:
            shmem_p -> message = "meltdown.";
            raise(SIGUSR1);
          break;

          case 0:
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
    char * vec_atomo[] = {"atomo", n_atom, id_shmat, NULL};

    switch(pid_atoms[i]) {

      case -1:
        shmem_p -> message = "meltdown.";
        raise(SIGUSR1);
      break;

      case 0: // children: freeing allocated memory
        free(pid_atoms);
        if (execve("./atomo", vec_atomo, NULL) == -1) {perror("Execve atomo"); exit(EXIT_FAILURE);}
      break;

      default:
        printf("Atom's pid is %d\n", pid_atoms[i]);
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
  alarm(SIM_DURATION);

  //? vogliamo metterla in shmem?
  //int available_en;

  // !!! cambiare condizione for 
  for(; 1; ) {
    print_stats(shmem_p);

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
    
    sleep(1);
  }
}


void print_stats(data_buffer * shmem_p) {
  static int count = 0;

  int col1_width = 35;
  int col2_width = 10;
  printf("\n\n\n\n");
  printf("STATS:\n");
  for (int i = 0; i <= (col1_width + col2_width); i++) {
    printf("-");
  }
  printf("\n");

  char *col1[11] = {"Last second activations:","Total activations:","Last second divisions:","Total divisions:",
    "Last second produced energy:","Total produced energy:","Last second consumed energy:","Total consumed energy:",
    "Last second waste:","Total waste:"};
  int col2[11] = {shmem_p -> act_rel, shmem_p -> act_tot, shmem_p -> div_rel, shmem_p -> div_tot, shmem_p -> prod_en_rel,
    shmem_p -> prod_en_tot, shmem_p -> cons_en_rel, shmem_p -> cons_en_tot, shmem_p -> waste_rel, shmem_p -> waste_tot};

  for (long unsigned int i = 0; i < (sizeof(col1)/sizeof(col1[0]))-1; i++) {
    printf("%-*s | %-*d\n", col1_width, col1[i], col2_width, col2[i]);
    if (i%2!=0) {
      for (int i = 0; i <= (col1_width + col2_width); i++) {
        printf("-");
      }
      printf("\n");
    }
  }

  printf("Simulation timer: %d\n", count++);
}

// ! metodo non usato ancora, da aggiustare per gestire i valori totali
void stat_total_value() {
  shmem_p -> waste_tot = shmem_p -> waste_tot + shmem_p -> waste_rel;
  shmem_p -> act_tot = shmem_p -> act_tot + shmem_p -> act_rel;
  shmem_p -> div_tot = shmem_p -> div_tot + shmem_p -> div_rel;
  shmem_p -> prod_en_tot = shmem_p -> prod_en_tot + shmem_p -> prod_en_rel;
  shmem_p -> cons_en_tot = shmem_p -> cons_en_tot + shmem_p -> cons_en_rel;
} // ! crea il ricevitore di dato e prova a stampare