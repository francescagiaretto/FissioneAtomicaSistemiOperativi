#include "library.h"

void signal_handler(int sig);
int semid, shmid, msgid;


int main(int argc, char * argv[]) {

   //! bisogna passargli l'id della shared memory perché altrimenti nel vec_atomo non glielo diamo
  int atomic_num;
  char n_atom[4];
  srand(getpid());

  shmid = atoi(argv[1]);
  semid = atoi(argv[2]);
  msgid = atoi(argv[3]);

  message_buffer * my_message;

  data_buffer * shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
  TEST_ERROR;


  //* STRUCT defines a nanosec-based sleep (can't be done with standard sleep())
  struct timespec step_nanosec;
  step_nanosec.tv_sec = 0;           // seconds   
  step_nanosec.tv_nsec = STEP_ALIMENTAZIONE;   // nanoseconds

  sem.sem_num = STARTSEM;
  sem.sem_op = -1;
  semop(semid, &sem, 1);
  CHECK_OPERATION;
  //printf("ALIMENTAZIONE: %d, shmid: %d, semid: %d\n\n", getpid(), shmid, semid);

 	//while(1);

  // TODO ogni STEP_ALIMENTAZIONE nanosecondi deve creare N_NUOVI_ATOMI
  //??? va bene while(1) o c'è un modo più elegante di scriverlo?
  while(1) {

    for(int i = 0; i < N_NUOVI_ATOMI; i++){
      atomic_num = rand() % N_ATOM_MAX + 1;
      sprintf(n_atom, "%d", atomic_num);
      char * vec_atomo[] = {"atomo", n_atom, argv[1], argv[2], argv[3], NULL}; // argv[1] = pointer to shmem
    
      switch(fork()) {

        case -1:
          shmem_ptr -> message = "meltdown";
          kill(getppid(), SIGUSR1);
        break;

        case 0:
          //printf("ALIMENTO NUOVI ATOMI\n");
          /* sprintf(my_message -> message, "%d", getpid());
          my_message -> type = MSGTYPE;
          msgsnd(msgid, &my_message, sizeof(pid_t), 0);
          TEST_ERROR; */

          execve("./atomo", vec_atomo, NULL);
          TEST_ERROR;
        break;

        default:
          // TODO inviare il pid del figlio all'attivatore
          nanosleep(&step_nanosec, NULL); // ricontrolla bene questo, se arriva un segnale va avanti, metti conttollo che riesca a riportarti ad aspettare del tempo
        break;
      } 
    } 
  } 
}

