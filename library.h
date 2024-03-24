#define _GNU_SOURCE
#include "parameters.txt"

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


#define CHECK_OPERATION switch (errno) {					\
		                        case EIDRM:						\
		                        	printf("PID = %d, riga:%d : semaphore removed while process was waiting\n", \
		                        	       getpid(), __LINE__);			\
		                        	exit(EXIT_FAILURE);				\
		                        case EINVAL:						\
		                        	printf("PID = %d, riga:%d : semaphore removed or never existed\n", \
		                        	       getpid(), __LINE__);			\
		                        	exit(EXIT_FAILURE);				\
		                        case EAGAIN:						\
		                        	printf("PID = %d, riga:%d : process continued without waiting\n", \
		                        	       getpid(), __LINE__);			\
		                        	exit(EXIT_FAILURE);				\
		                        				\
		                        default:						\
		                        	TEST_ERROR;			\
                            break;    \
		                        }							\

struct sembuf sem;


typedef struct message {
    long type;
    char message[128];
} message_buffer;

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
  int termination; // if set to 1 children processes are killed
  int attiv_signal;
} data_buffer;




