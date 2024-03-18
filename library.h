#define _GNU_SOURCE
#include "parameters.txt"

#define WAITSEM 0
#define STARTSEM 1
#define WASTESEM 2

#define MAX(a, b) ((a > b) ? (a) : (b))
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>


#define TEST_ERROR   if (errno) {fprintf(stderr, \
					   "%s:%d: PID=%5d: Error %d (%s)\n",\
					   __FILE__,\
					   __LINE__,\
					   getpid(),\
					   errno,\
					   strerror(errno));}

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

void print_stats(data_buffer * shmem_ptr) {
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
  int col2[11] = {shmem_ptr -> act_rel, shmem_ptr -> act_tot, shmem_ptr -> div_rel, shmem_ptr -> div_tot, shmem_ptr -> prod_en_rel,
    shmem_ptr -> prod_en_tot, shmem_ptr -> cons_en_rel, shmem_ptr -> cons_en_tot, shmem_ptr -> waste_rel, shmem_ptr -> waste_tot};

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

void stat_total_value(data_buffer * shmem_ptr) {
  shmem_ptr -> waste_tot = shmem_ptr -> waste_tot + shmem_ptr -> waste_rel;
  shmem_ptr -> act_tot = shmem_ptr -> act_tot + shmem_ptr -> act_rel;
  shmem_ptr -> div_tot = shmem_ptr -> div_tot + shmem_ptr -> div_rel;
  shmem_ptr -> prod_en_tot = shmem_ptr -> prod_en_tot + shmem_ptr -> prod_en_rel;
  shmem_ptr -> cons_en_tot = shmem_ptr -> cons_en_tot + shmem_ptr -> cons_en_rel;
}

int energy(int child, int parent) {
    return child*parent - MAX(child,parent);
}

/* void check_error() {
  if (errno) {
    fprintf(stderr, "line %d in file '%s': Error: %d (%s)\n", 
      __LINE__, __FILE__, errno, strerror(errno));
    
    exit(EXIT_FAILURE);
  }
} */

void check_waste(int atom_num, data_buffer * shmem_ptr) {
  if (atom_num <= MIN_N_ATOMICO) { 
		shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;
		kill(getpid(), SIGTERM);
	} 
} 
