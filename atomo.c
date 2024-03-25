#include "library.h"

void generate_n_atom(int *, int *);
void operate_in_sem(int, int);
int energy(int, int);

int semid, shmid, msgid;
data_buffer * shmem_ptr;

int main(int argc, char* argv[]){

	int child_atom_num, en_lib, parent_atom_num;
	char division_atom_num[3], division_parent_num[4];

	parent_atom_num = atoi(argv[1]);
	shmid = atoi(argv[2]);
	semid = atoi(argv[3]);
	msgid = atoi(argv[4]);

	shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

	if (shmem_ptr -> termination == 1) {
		int status = 0;
		waitpid(-1, &status, WIFEXITED(status));
		kill(getpid(), SIGTERM);
	}

	srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be
									//*  identical for more than one atom, while pid is always different


	if(shmem_ptr -> simulation_start == 1) {operate_in_sem(STARTSEM, 0);}
	shmem_ptr -> simulation_start = 0;

	//* il controllo delle scorie è fatto dopo che il processo atomo ha ricevuto il comando di scissione
	if(parent_atom_num <= MIN_N_ATOMICO) { 
		operate_in_sem(WASTESEM, 0);
		raise(SIGTERM);
	}
	
	generate_n_atom(&parent_atom_num, &child_atom_num);

	// TODO gestire la fork quando lo richiede l'attivatore.

	sprintf(division_atom_num, "%d", child_atom_num);
	char * vec_atomo[] = {"atomo", division_atom_num, argv[2], argv[3], NULL};

	operate_in_sem(ACTIVATIONSEM, 0);
	switch (fork())
	{
		case -1:
			shmem_ptr -> message = "meltdown";
			kill(getppid(), SIGUSR1);
		break;
		
		case 0: // checking child
			operate_in_sem(DIVISIONSEM, 0);
			execve("atomo", vec_atomo, NULL);
			TEST_ERROR;
		break;
		
		default: // checking parent
			en_lib = energy(child_atom_num, parent_atom_num);
			operate_in_sem(PROD_ENERGYSEM, en_lib); // saving relative produced energy in shmem
			sprintf(division_parent_num, "%d", parent_atom_num);
			char * new_vec_atomo[] = {"atomo", division_parent_num, argv[2], argv[3], NULL};
			execve("atomo", new_vec_atomo, NULL);
			TEST_ERROR;
		break;
	}
}


void generate_n_atom(int * parent_atom_num, int * child_atom_num) {
  int temp = *parent_atom_num;
  *parent_atom_num = rand() % *parent_atom_num + 1;
  *child_atom_num = temp - *parent_atom_num;
}

void operate_in_sem(int sem_working, int en_lib){

	switch(sem_working) {
		case STARTSEM:
			sem.sem_num = STARTSEM;
			sem.sem_op = -1;
  			semop(semid, &sem, 1);
		break;

		case WASTESEM:
			sem.sem_num = WASTESEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;

			shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

			sem.sem_num = WASTESEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;
		break;

		case PROD_ENERGYSEM:
			sem.sem_num = PROD_ENERGYSEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;

			shmem_ptr -> prod_en_rel = shmem_ptr -> prod_en_rel + en_lib;

			sem.sem_num = PROD_ENERGYSEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;
		break;

		case DIVISIONSEM:
			sem.sem_num = DIVISIONSEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;

			shmem_ptr -> div_rel = shmem_ptr -> div_rel + 1;

			sem.sem_num = DIVISIONSEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;
		break;

		case ACTIVATIONSEM:
			sem.sem_num = ACTIVATIONSEM;
			sem.sem_op = -1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;

			shmem_ptr -> act_rel = shmem_ptr -> act_rel + 1;

			sem.sem_num = ACTIVATIONSEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			CHECK_OPERATION;
		break;
	}

}

int energy(int child, int parent) {
    return child*parent - MAX(child,parent);
}