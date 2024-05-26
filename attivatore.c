#include "library.h"

int semid, shmid, msgid;

void signal_handler(int sig) {
	switch(sig) {
		case SIGQUIT:
		break;
	}
}

int main(int argc, char* argv[]) {
	pid_t new_pid;
	semid = atoi(argv[1]);
	shmid = atoi(argv[2]);
	msgid = atoi(argv[3]);

	data_buffer * shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

	struct timespec step_nanosec;
  	step_nanosec.tv_sec = 0;          
  	step_nanosec.tv_nsec = STEP_ATTIVATORE;  

	struct sigaction sa;
  	bzero(&sa, sizeof(sa)); 
  	sa.sa_handler = &signal_handler;
  	sa.sa_flags = SA_RESTART;
  	sigaction(SIGQUIT, &sa, NULL);

	sem.sem_num = STARTSEM;
 	sem.sem_op = -1;
  semop(semid, &sem, 1);

  //! fare aggiustamenti e capire se le attivazioni seguono una logica con gli atomi

	while (shmem_ptr -> termination != 1) {
		nanosleep(&step_nanosec, NULL);

		//! riceviamo il pid di ogni atomo
		do { 
			new_pid = receive_pid(msgid);
		} while(new_pid == -1 && errno == EINTR);

		if (shmem_ptr -> inib_on == 1 && shmem_ptr -> remainder == new_pid % 2) {
			//printf("Ho impedito la scissione di %d, avente resto %d\n", new_pid, shmem_ptr -> remainder);
			kill(new_pid, SIGTERM);
			sem.sem_num = WASTESEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);
			//CHECK_OPERATION;

			shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

			sem.sem_num = WASTESEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			//CHECK_OPERATION;

			shmem_ptr -> undiv_rel = shmem_ptr -> undiv_rel + 1;
		} else {
			//!segnale di scissione
			//printf("scissione di %d\n", new_pid);
			shmem_ptr -> act_rel = shmem_ptr -> act_rel + 1;
			kill(new_pid, SIGUSR2);
		}
	}
}