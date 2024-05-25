#include "library.h"

int semid, shmid, msgid;

int main(int argc, char* argv[]) {
	pid_t new_pid;
	semid = atoi(argv[1]);
	shmid = atoi(argv[2]);
	msgid = atoi(argv[3]);

	data_buffer * shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

	struct timespec step_nanosec;
  	step_nanosec.tv_sec = 0;           // seconds   
  	step_nanosec.tv_nsec = STEP_ATTIVATORE;   // nanoseconds
	
	sem.sem_num = STARTSEM;
 	sem.sem_op = -1;
  	semop(semid, &sem, 1);

	while (shmem_ptr -> termination != 1) {
		nanosleep(&step_nanosec, NULL);

		//! riceviamo il pid di ogni atomo
		new_pid = receive_pid(msgid);
		if(new_pid == -1) {
			perror("pid attivatore");
		exit(EXIT_FAILURE);
		}
		
		//!segnale di scissione
		shmem_ptr -> act_rel = shmem_ptr -> act_rel + 1;
		kill(new_pid, SIGUSR2);
	}
}