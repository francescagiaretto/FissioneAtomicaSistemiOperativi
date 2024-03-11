#define _GNU_SOURCE
#include "parameters.txt"
#include "methods.c"

#define WAITSEM 0
#define ATTIVATORE 1

/**
 * Struct to handle semaphore ops
*/
struct sembuf sem;


/**
 * Shared memory: stats and termination reason
*/
typedef struct data_buffer data_buffer;

/**
 * Prints stats from shared memory
*/
void print_stats(data_buffer *);

/**
 * Calculates the value for total stats from relative values
*/
void stat_total_value();

/**
 * Calculates energy produced during divisions
*/
int energy(int, int);




//! LI TENIAMO?

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