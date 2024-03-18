#include "library.h"

void generate_n_atom(int *, int *);
void check_waste(data_buffer *);
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

	srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be
									//*  identical for more than one atom, while pid is always different

	sem.sem_num = STARTSEM;
	sem.sem_op = -1;
  	semop(semid, &sem, 1);
	// printf("ATOMO CON NUM ATOMICO [%d]\n\n", parent_atom_num);
	fflush(stdout);

	//* il controllo delle scorie è fatto dopo che il processo atomo ha ricevuto il comando di scissione
	if(parent_atom_num <= MIN_N_ATOMICO) { 

		sem.sem_op = WASTESEM;
		sem.sem_op = -1;
		semop(semid, &sem, 1);
		CHECK_OPERATION;

		printf("sono processo con pid %d e sto scrivendo per le scorie\n", getpid());
		fflush(stdout);
		shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

		printf("sono processo con pid %d e sto liberando le risorse le scorie\n", getpid());+
		fflush(stdout);
		sem.sem_op = WASTESEM;
		sem.sem_op = 1;
		semop(semid, &sem, 1);
		CHECK_OPERATION;;

		raise(SIGTERM);
	}
	
	generate_n_atom(&parent_atom_num, &child_atom_num);

	// TODO gestire la fork quando lo richiede l'attivatore.

	sprintf(division_atom_num, "%d", child_atom_num);
	char * vec_atomo[] = {"atomo", division_atom_num, argv[2], argv[3], NULL};

	// TODO funzione energy() che incrementa l'energia liberata nelle statistiche del master
		switch (fork())
		{
			case -1:
				shmem_ptr -> message = "meltdown.";
				kill(getppid(), SIGUSR1);
			break;
			
			case 0: // checking child
				shmem_ptr -> div_rel = shmem_ptr -> div_rel + 1;
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

/* void check_waste(data_buffer * shmem_ptr) {

		printf("TEST %d\n\n", getpid());
		sem.sem_op = WASTESEM;
		sem.sem_op = -1;
		semop(semid, &sem, 1);
		CHECK_OPERATION;
		printf("TEST 2  %d\n\n", getpid());

		printf("sono processo con pid %d e sto scrivendo le scorie\n", getpid());
		shmem_ptr -> waste_rel = shmem_ptr -> waste_rel +1;

		printf("sono processo con pid %d e sto liberando le risorse le scorie\n", getpid());
		sem.sem_op = WASTESEM;
		sem.sem_op = 1;
		semop(semid, &sem, 1);
		CHECK_OPERATION;;

		kill(getpid(), SIGTERM);
}  */