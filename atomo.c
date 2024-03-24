#include "library.h"

void generate_n_atom(int *, int *);
void operate_in_sem(int, data_buffer*);
int semid, shmid;


int main(int argc, char* argv[]){

	int child_atom_num, en_lib, parent_atom_num;
	data_buffer * shmem_ptr;
	char division_atom_num[3], division_parent_num[4];

	parent_atom_num = atoi(argv[1]);
	shmid = atoi(argv[2]);
	semid = atoi(argv[3]);
	shmem_ptr = (data_buffer *) shmat(shmid, NULL, 0);
	TEST_ERROR;

	if (shmem_ptr -> termination == 1) {
		int status = 0;
		waitpid(-1, &status, WIFEXITED(status));
		kill(getpid(), SIGTERM);
	}

	srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be
									//*  identical for more than one atom, while pid is always different

	sem.sem_num = STARTSEM;
	sem.sem_op = -1;
  	semop(semid, &sem, 1);
	printf("ATOMO %d CON NUM ATOMICO [%d]\n\n", getpid(), parent_atom_num);

	//* il controllo delle scorie è fatto dopo che il processo atomo ha ricevuto il comando di scissione
	if(parent_atom_num <= MIN_N_ATOMICO) { 

		operate_in_sem(WASTESEM, shmem_ptr);
		/* sem.sem_num = WASTESEM;
		sem.sem_op = -1;
		printf("SONO FERMO QUI\n");
		semop(semid, &sem, 1);
		printf("SONO LIBERATO\n");
		CHECK_OPERATION;

		printf("sono processo con pid %d e sto scrivendo per le scorie\n", getpid());
		shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

		printf("sono processo con pid %d e sto liberando le risorse le scorie\n", getpid());
		sem.sem_num = WASTESEM;
		sem.sem_op = 1;
		semop(semid, &sem, 1);
		CHECK_OPERATION;


		raise(SIGTERM); */
		raise(SIGTERM);
	}
	
	generate_n_atom(&parent_atom_num, &child_atom_num);

	// TODO gestire la fork quando lo richiede l'attivatore.

	sprintf(division_atom_num, "%d", child_atom_num);
	char * vec_atomo[] = {"atomo", division_atom_num, argv[2], argv[3], NULL};

	//if (shmem_ptr -> attiv_signal == 1) {switch che segue}?
	switch (fork())
	{
		case -1:
			shmem_ptr -> message = "meltdown.";
			kill(getppid(), SIGUSR1);
		break;
		
		case 0: // checking child
			operate_in_sem(DIVISIONSEM, shmem_ptr);
			execve("atomo", vec_atomo, NULL);
			TEST_ERROR;
		break;
		
		default: // checking parent
			en_lib = energy(child_atom_num, parent_atom_num);
			shmem_ptr -> prod_en_rel = shmem_ptr -> prod_en_rel + en_lib; // saving relative produced energy in shmem
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

void operate_in_sem(int sem_working, data_buffer * shmem_ptr){

	switch(sem_working) {
		case WASTESEM:
			sem.sem_num = WASTESEM;
			sem.sem_op = -1;
			printf("pid %d, entrando in attesa per le scorie\n", getpid());
			semop(semid, &sem, 1);
			CHECK_OPERATION;

			shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

			sem.sem_num = WASTESEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			printf("pid %d e ho liberato le risorse per le SCORIE\n", getpid());
			CHECK_OPERATION;
		break;

		case PROD_ENERGYSEM:
		break;

		case DIVISIONSEM:
			sem.sem_num = DIVISIONSEM;
			sem.sem_op = -1;
			printf("pid %d, entrando in attesa per le divisioni\n", getpid());
			semop(semid, &sem, 1);
			printf("pid %d, uscito dall'attesa per le divisioni\n", getpid());
			printf("valore semaforo: %d\n", semctl(semid, DIVISIONSEM, GETVAL));
			CHECK_OPERATION;

			shmem_ptr -> div_rel = shmem_ptr -> div_rel + 1;

			sem.sem_num = DIVISIONSEM;
			sem.sem_op = 1;
			semop(semid, &sem, 1);
			printf("pid %d, ho liberato le risorse per le divisioni\n", getpid());
			CHECK_OPERATION;
		break;

		case ACTIVATIONSEM:
		break;
	}

}