#include "library.h"

void set_sem_values();
void print_stats();
void stat_total_value();

int shmid, semid, available_en, msgid;
data_buffer * shmem_ptr;
pid_t pid_alimentazione, pid_attivatore, pid_inibitore;

void signal_handler(int sig) {
  switch(sig) {
    case SIGALRM:
      shmem_ptr -> termination = 1;
      shmem_ptr -> message = "timeout.";
    break;

    case SIGUSR2:
      write(0, "Simulation terminated due to meltdown.\n", 39);
      kill(pid_alimentazione, SIGTERM);
      kill(pid_attivatore, SIGTERM);
      kill(pid_inibitore, SIGTERM);
      semctl(semid, 0, IPC_RMID);
      shmdt(shmem_ptr);
      shmctl(shmid, IPC_RMID, NULL);
      msgctl(msgid, IPC_RMID, NULL);
      killpg(getpgid(getpid()), SIGTERM);
    break;

    //! dà problemi quando si accende l'inibitore a inizio simulazione, lo si spegne, alla seconda riaccensione dà on off
    case SIGUSR1:
      if (shmem_ptr -> inib_on == 1) {
        kill(pid_inibitore, SIGQUIT); 
        TEST_ERROR;
      } else if(shmem_ptr -> inib_on == 0){
        shmem_ptr -> inib_on = 1;
        
        sem.sem_num = INIBSEM;
        sem.sem_op = 1;
        semop(semid, &sem, 1); 

        write(0, "Inibitore ON. Turn off anytime with ctrl + backslash \n", 55);
      }
    break;

    case SIGINT:
      shmem_ptr -> termination  = 1;
      shmem_ptr -> message = "user.";
    break;

    case SIGQUIT:
    break;

    case SIGSEGV:
      shmem_ptr -> termination  = 1;
      shmem_ptr -> message = "segmentation fault.";
    break;
  }
}

int main(int argc, char* argv[]) {

  int atomic_num;
  pid_t * pid_atoms;
  key_t shmkey, semkey, msgkey;
  char n_atom[8], id_shmat[8], pointer_shmem[8], id_sem[8], id_message[8];
  
  srand(getpid());

  shmkey = ftok("master.c", 'A');
  semkey = ftok("master.c", 'B');
  msgkey = ftok("master.c", 'C');

  shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
  TEST_ERROR;

  semid = semget(semkey, 7, IPC_CREAT | 0666);
  TEST_ERROR;

  msgid = msgget(msgkey, IPC_CREAT | 0666);
  TEST_ERROR;

  shmem_ptr = (data_buffer *)shmat(shmid, NULL, 0); // NULL for improved code portability: address may not be available outside of Unix
  TEST_ERROR;

  shmem_ptr -> pid_master = getpid();
  shmem_ptr -> termination  = 0;

  set_sem_values();
  sem.sem_flg = 0;

  //printf("%d in master all'inizio\n", semctl(semid, INIBSEM, GETVAL));

  sprintf(id_shmat, "%d", shmid);
  sprintf(id_sem, "%d", semid);
  sprintf(id_message, "%d", msgid);
  char * vec_alim[] = {"./alimentazione", id_shmat, id_sem, id_message, NULL};
  char * vec_attiv[] = {"./attivatore", id_sem, id_shmat, id_message, NULL};
  char * vec_inib[] = {"./inibitore", id_sem, id_shmat, id_message, NULL};

  switch(pid_alimentazione = fork()) {
    case -1:
      sem.sem_num = MELTDOWNSEM;
      sem.sem_op = -1;
      semop(semid, &sem, 1);

      shmem_ptr->message = "meltdown.";
      raise(SIGUSR2);
    break;

    case 0:
      /* sem.sem_num = WAITSEM;
      sem.sem_op = 1;
      semop(semid, &sem, 1); */
      execve("./alimentazione", vec_alim, NULL);
      TEST_ERROR;
    break;

    default: // parent process
      switch(pid_attivatore = fork()) {

        case -1:
          sem.sem_num = MELTDOWNSEM;
          sem.sem_op = -1;
          semop(semid, &sem, 1); 
          
          shmem_ptr->message = "meltdown.";
          raise(SIGUSR2);
        break;

        case 0:
          /* sem.sem_num = WAITSEM;
          sem.sem_op = 1;
          semop(semid, &sem, 1); */
          execve("./attivatore", vec_attiv, NULL);
          TEST_ERROR;
        break;
        
        default:
        //! attivazione inibitore
        /*
        l'inibitore deve rimanere attivo per non perdere tempo a disallocare e riallocare le risorse a ogni attivazione,
        quindi impostiamo un semaforo dedicato INIBSEM che occupa la risorsa mentre l'inibitore non è attivo e la libera
        quando ne viene richiesta l'attivazione (così evitiamo busy waiting e cicli infiniti in attesa di attivazione)
        */
          switch(pid_inibitore = fork()) {
            case -1:
              sem.sem_num = MELTDOWNSEM;
              sem.sem_op = -1;
              semop(semid, &sem, 1);

              shmem_ptr->message = "meltdown.";
              raise(SIGUSR2);
            break;
            
            case 0:
              /* sem.sem_num = WAITSEM;
              sem.sem_op = 1;
              semop(semid, &sem, 1); */
              execve("./inibitore", vec_inib, NULL);
              TEST_ERROR;
            break;
          }
      }
    break;
  } 
  
  shmem_ptr -> simulation_start = 1;
  pid_atoms = malloc(sizeof(pid_t) * N_ATOM_INIT);    // dynamic mem allocated for pid atomi array
  for(int i = 0; i < N_ATOM_INIT; i++) {

    pid_atoms[i] = fork();

    // generating random pid for children
    srand(getpid());
    atomic_num = rand() % N_ATOM_MAX + 1;
    sprintf(n_atom, "%d", atomic_num);
    char * vec_atomo[] = {"./atomo", n_atom, id_shmat, id_sem, id_message, NULL};

    switch(pid_atoms[i]) {

      case -1:
        sem.sem_num = MELTDOWNSEM;
        sem.sem_op = -1;
        semop(semid, &sem, 1);

        shmem_ptr->message = "meltdown.";
        raise(SIGUSR2);
      break;

      case 0: 
        free(pid_atoms);
        /* sem.sem_num = WAITSEM;
        sem.sem_op = 1;
        semop(semid, &sem, 1);*/

        execve("./atomo", vec_atomo, NULL);
        TEST_ERROR;
      break;

      default:
      break;
    }
  }

  fflush(stdout);
  char risposta;
  do {
    printf("Do you want to turn inibitore on? (y for yes, n for no)\n");
    scanf("%s", &risposta);
  } while(tolower(risposta) != 'y' && tolower(risposta) != 'n');

  struct sigaction sa;

  bzero(&sa, sizeof(sa)); // emptying struct to send to child
  sa.sa_handler = &signal_handler;
  sigaction(SIGALRM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);

  free(pid_atoms);

  /* sem.sem_num = WAITSEM;
	sem.sem_op = -(N_ATOM_INIT + 3);
  semop(semid, &sem, 1); */
  
  sem.sem_num = STARTSEM;
  sem.sem_op = N_ATOM_INIT +3;
  semop(semid, &sem, 1);

  alarm(SIM_DURATION);

  if (tolower(risposta) == 'y') {
    raise(SIGUSR1);
  }

  while(shmem_ptr -> termination != 1) {
    sleep(1);

    shmem_ptr -> cons_en_rel = ENERGY_DEMAND;
    
    stat_total_value();

    // checking explode condition
    if (shmem_ptr -> prod_en_tot  >= ENERGY_EXPLODE_THRESHOLD) {
      shmem_ptr -> message = "explode.";
      raise(SIGTSTP);
    }

    print_stats();
    bzero(shmem_ptr, 7*sizeof(int));

    // checking blackout condition
    if (ENERGY_DEMAND > shmem_ptr -> prod_en_tot) {
      shmem_ptr->message = "blackout.";
      raise(SIGTSTP);
    } else {
      shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot - ENERGY_DEMAND;
    }
  }

  printf("Simulation terminated due to %s\n", shmem_ptr -> message);
  semctl(semid, 0, IPC_RMID);
  shmdt(shmem_ptr);
  shmctl(shmid, IPC_RMID, NULL);
  msgctl(msgid, IPC_RMID, NULL);
  killpg(getpgid(getpid()), SIGTERM);
}

void set_sem_values(){
  
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

  // handles inibitore
  semctl(semid, INIBSEM, SETVAL, 0);
  TEST_ERROR;

  // handles meltdown termination
  semctl(semid, MELTDOWNSEM, SETVAL, 1);
  TEST_ERROR;
}


void print_stats() {
  static int count = 1;

  int col1_width = 35;
  int col2_width = 15;
  printf("\n\n\n\n");
  printf("STATS:\n");
  for (int i = 0; i <= (col1_width + col2_width); i++) {
    printf("-");
  }
  printf("\n");

  char *col1[15] = {"Last second activations:","Total activations:","Last second divisions:","Total divisions:",
    "Last second produced energy:","Total produced energy:","Last second consumed energy:","Total consumed energy:",
    "Last second waste:","Total waste:","Last second absorbed energy:","Total absorbed energy:",
    "Last second undivided atoms:","Total undivided atoms:"};
  int col2[15] = {shmem_ptr -> act_rel, shmem_ptr -> act_tot, shmem_ptr -> div_rel, shmem_ptr -> div_tot, shmem_ptr -> prod_en_rel,
    shmem_ptr -> prod_en_tot, shmem_ptr -> cons_en_rel, shmem_ptr -> cons_en_tot, shmem_ptr -> waste_rel, shmem_ptr -> waste_tot,
    shmem_ptr -> absorbed_en_rel, shmem_ptr -> absorbed_en_tot, shmem_ptr -> undiv_rel, shmem_ptr -> undiv_tot};

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

void stat_total_value() {
  shmem_ptr -> waste_tot = shmem_ptr -> waste_tot + shmem_ptr -> waste_rel;
  shmem_ptr -> act_tot = shmem_ptr -> act_tot + shmem_ptr -> act_rel;
  shmem_ptr -> div_tot = shmem_ptr -> div_tot + shmem_ptr -> div_rel;
  shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot + shmem_ptr -> prod_en_rel;
  shmem_ptr -> cons_en_tot = shmem_ptr -> cons_en_tot + shmem_ptr -> cons_en_rel;
  shmem_ptr -> absorbed_en_tot = shmem_ptr -> absorbed_en_tot + shmem_ptr -> absorbed_en_rel;
  shmem_ptr -> undiv_tot = shmem_ptr -> undiv_tot + shmem_ptr -> undiv_rel;
}