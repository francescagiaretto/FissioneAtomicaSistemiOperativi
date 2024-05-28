#include "library.h"

void generate_n_atom(int *, int *);
void operate_in_sem(int, int);
int energy(int, int);

int semid, shmid, msgid, atom_activation;
data_buffer * shmem_ptr;

void signal_handler(int sig) {
	switch(sig) {
		case SIGUSR2:
			atom_activation = 1;
		break;

		case SIGSEGV:
    	kill(shmem_ptr -> pid_master, SIGSEGV);
    break;
	}
}

int main(int argc, char* argv[]){

	int child_atom_num, en_lib, parent_atom_num;
	char division_atom_num[3], division_parent_num[4];

	atom_activation = 0;

	parent_atom_num = atoi(argv[1]);
	shmid = atoi(argv[2]);
	semid = atoi(argv[3]);
	msgid = atoi(argv[4]);

	shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

	struct sigaction sa;
	sigset_t mymask;

	bzero(&sa, sizeof(sa));
	sa.sa_handler = &signal_handler;
	sa.sa_mask = mymask;

	sigemptyset(&mymask);
	sigaddset(&mymask, SIGQUIT);
	sigprocmask(SIG_BLOCK, &mymask, NULL);
  	sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);

	srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be identical for more than one atom, while pid is always different

	if(shmem_ptr -> simulation_start == 1) {operate_in_sem(STARTSEM, 0); }
	printf("atomo\n");

	if(parent_atom_num <= MIN_N_ATOMICO) { 
		operate_in_sem(WASTESEM, 0);
		exit(0);
	}

	//! ogni atomo manda all'attivatore il suo pid
	send_atom_pid(msgid, getpid());

	//! pause() mette in waiting il programma finche non gli viene lanciato un qualsiasi segnale
	pause();

	generate_n_atom(&parent_atom_num, &child_atom_num);
	sprintf(division_atom_num, "%d", child_atom_num);
	char * vec_atomo[] = {"atomo", division_atom_num, argv[2], argv[3], NULL};

	if (atom_activation == 1) {
		switch (fork()) {
			case -1:
				sem.sem_num = MELTDOWNSEM;
				sem.sem_op = -1;
				semop(semid, &sem, 1);
				kill(shmem_ptr -> pid_master, SIGUSR2);
			break;

			case 0: // checking child
				operate_in_sem(DIVISIONSEM, 0);
				execve("./atomo", vec_atomo, NULL);
				TEST_ERROR;
			break;

			default: // checking parent
				en_lib = energy(child_atom_num, parent_atom_num);

				operate_in_sem(PROD_ENERGYSEM, en_lib); // saving relative produced energy in shmem

				sprintf(division_parent_num, "%d", parent_atom_num);
				char * new_vec_atomo[] = {"./atomo", division_parent_num, argv[2], argv[3], NULL};
				execve("atomo", new_vec_atomo, NULL);
				TEST_ERROR;
			break;
		}
	}
}


void generate_n_atom(int * parent_atom_num, int * child_atom_num) {
  int temp = *parent_atom_num;
  *parent_atom_num = (rand() % *parent_atom_num) + 1;
  *child_atom_num = temp - *parent_atom_num;
}

void operate_in_sem(int sem_working, int en_lib){

	switch(sem_working) {
		case STARTSEM:
			sem.sem_num = STARTSEM;
			sem.sem_op = -1;
  			semop(semid, &sem, 1);

			shmem_ptr -> simulation_start = 0;
		break;

		case WASTESEM:
			sem.sem_num = WASTESEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);

			shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

			sem.sem_num = WASTESEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
		break;

		case PROD_ENERGYSEM:
			sem.sem_num = PROD_ENERGYSEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);

			shmem_ptr -> prod_en_rel = shmem_ptr -> prod_en_rel + en_lib;

			sem.sem_num = PROD_ENERGYSEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
		break;

		case DIVISIONSEM:
			sem.sem_num = DIVISIONSEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);

			shmem_ptr -> div_rel = shmem_ptr -> div_rel + 1;

			sem.sem_num = DIVISIONSEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
		break;
	}
}

int energy(int child, int parent) {
    return child*parent - MAX(child,parent);
}