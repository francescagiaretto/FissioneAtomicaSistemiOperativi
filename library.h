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
#include <ctype.h>

#define N_ATOM_INIT 10
#define N_ATOM_MAX 118
#define ENERGY_DEMAND 2000
#define STEP_ATTIVATORE 90000000
#define N_NUOVI_ATOMI 10
#define SIM_DURATION 5
#define ENERGY_EXPLODE_THRESHOLD 100000000
#define MIN_N_ATOMICO 30
#define STEP_ALIMENTAZIONE 8
#define SHM_SIZE 500
#define PID_TYPE 1
#define WASTE_TYPE 2

#define WAITSEM 0
#define STARTSEM 1
#define WASTESEM 2
#define PROD_ENERGYSEM 3
#define DIVISIONSEM 4
#define ACTIVATIONSEM 5
#define INIBSEM 6


#define TEST_ERROR   if (errno) {fprintf(stderr, \
					   "%s:%d: PID=%5d: Error %d (%s)\n",\
					   __FILE__,\
					   __LINE__,\
					   getpid(),\
					   errno,\
					   strerror(errno));}


//#define CHECK_OPERATION switch (errno) { \
		                        case EIDRM: \
		                        	printf("PID = %d, riga:%d : semaphore removed while process was waiting\n", \
		                        	       getpid(), __LINE__); \
		                        	exit(EXIT_FAILURE); \
		                        case EINVAL: \
		                        	printf("PID = %d, riga:%d : semaphore removed or never existed\n", \
		                        	       getpid(), __LINE__); \
		                        	exit(EXIT_FAILURE); \
		                        case EAGAIN: \
		                        	printf("PID = %d, riga:%d : process continued without waiting\n", \
		                        	       getpid(), __LINE__); \
		                        	exit(EXIT_FAILURE); \
		                        		\
		                        default: \
		                        	TEST_ERROR; \
                            break; \
		                        } \

struct sembuf sem;


typedef struct message {
    long mtype;
    char message[10];
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
  int attiv_signal;
  int simulation_start;
  int inib_on;
  int termination;
  pid_t pid_master;
} data_buffer;

int new_waste(int msg_queue, int pid){
    message_buffer mymessage;
    mymessage.mtype = WASTE_TYPE;
    sprintf(mymessage.message, "%d" , pid);
    return msgsnd(msg_queue, &mymessage, sizeof(mymessage) - sizeof(long), 0); 
}

int waste_update(int msg_queue, int *pid){
    message_buffer mymessage;
    int pid_received = msgrcv(msg_queue, &mymessage, sizeof(mymessage) - sizeof(long), WASTE_TYPE, IPC_NOWAIT);
    if(pid_received != -1)
        *pid = atoi(mymessage.message);
    return pid_received;
}

int new_atom(int msg_queue, int pid){
    message_buffer mymessage;
    mymessage.mtype = PID_TYPE;
    sprintf(mymessage.message, "%d" , pid);
    return msgsnd(msg_queue, &mymessage, sizeof(mymessage) - sizeof(long), 0); 
}

int atom_update(int msg_queue, int *pid){
    message_buffer mymessage;
    int pid_received = msgrcv(msg_queue, &mymessage, sizeof(mymessage) - sizeof(long), PID_TYPE, IPC_NOWAIT);
    if(pid_received != -1)
        *pid = atoi(mymessage.message);
    return pid_received;
}