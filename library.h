#define _GNU_SOURCE
#define MAX(a, b) ((a > b) ? (a) : (b))
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "parameters.txt"
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define WAITSEM 0
#define ATTIVATORE 1

struct sembuf sem;

typedef struct data_buffer {
  int waste_rel;
  int prod_en_rel;
  int div_rel;
  int act_rel;
  int cons_en_rel;
  int waste_tot;
  int prod_en_tot;
  int div_tot;
  int act_tot;
  int cons_en_tot;
  char * message; // termination message
} data_buffer;

/* int createSem(key_t key, int semnum) {
  return semget(key, semnum, IPC_CREAT | 0666);
}

int initSem(int semid, int semnum, int arg) {
  return semctl(semid, semnum, SETVAL, arg);
}

int reserveSem(int semid, int semnum, int val) {
  struct sembuf sem;
  sem.sem_num = semnum;
  sem.sem_op = -1;
  sem.sem_flg = 0;
  return semop(semid, &sem, val);
}

int releaseSem(int semid, int semnum) {
  struct sembuf sem;
  sem.sem_num = semnum;
  sem.sem_op = 1;
  sem.sem_flg = 0;
  return semop(semid, &sem, 1);
} */

void print_stats(data_buffer * shmem_p) {
  static int count = 1;

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

void signal_handler(int sig) {
  switch(sig) {
    case SIGALRM:
      shmem_p -> message = "timeout.";
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