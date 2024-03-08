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

typedef struct data_buffer {
  int position;
  int waste_rel;
  int waste_tot;
  int prod_en_rel;
  int prod_en_tot;
  int div_rel;
  int div_tot;
  int act_rel;
  int act_tot;
  int cons_en_rel;
  int cons_en_tot;
  char * message; // termination message
} data_buffer;

