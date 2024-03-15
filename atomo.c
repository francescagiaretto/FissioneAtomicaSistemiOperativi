#include "library.h"

void generate_n_atom(int *, int *);
int semid;


int main(int argc, char* argv[]){

	int parent_atom_num = atoi(argv[1]); int child_atom_num, en_lib, shmid;
	int key = atoi(argv[2]);
	data_buffer * shmem_p;
	char division_atom_num[3], division_parent_num[4];


	semid = semget(atoi(argv[3]), 2, IPC_CREAT | 0666);
	check_error(errno);
	shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
	check_error(errno);
	shmem_p = (data_buffer *) shmat(key, NULL, 0);
	check_error(errno);

	check_waste(parent_atom_num);	

	srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be
									//*  identical for more than one atom, while pid is always different

	/* em.sem_num = STARTSEM;
	sem.sem_op = -1;
  	if (semop(semid, &sem, 1) == -1){perror("semop startsem in atomo"); exit(EXIT_FAILURE);} 
	printf("\n\n\nTEST ATOMO\n\n\n"); */

	generate_n_atom(&parent_atom_num, &child_atom_num);

	// TODO gestire la fork quando lo richiede l'attivatore.

	sprintf(division_atom_num, "%d", child_atom_num);
	char * vec_atomo[] = {"atomo", division_atom_num, argv[2], NULL};

	// TODO funzione energy() che incrementa l'energia liberata nelle statistiche del master
		switch (fork())
		{
			case -1:
				shmem_p -> message = "meltdown.";
				kill(getppid(), SIGUSR1);
			break;
			
			case 0: // checking child
				shmem_p -> div_rel = shmem_p -> div_rel + 1;
				execve("atomo", vec_atomo, NULL);
				check_error(errno);
			break;
			
			default: // checking parent
				en_lib = energy(child_atom_num, parent_atom_num);
				shmem_p -> prod_en_rel = shmem_p -> prod_en_rel + en_lib; // saving relative produced energy in shmem

				sprintf(division_parent_num, "%d", parent_atom_num);
				char * new_vec_atomo[] = {"atomo", division_parent_num, argv[2], NULL};
				execve("atomo", new_vec_atomo, NULL);
				check_error(errno);
			break;
		}
}


void generate_n_atom(int * parent_atom_num, int * child_atom_num) {
  int temp = *parent_atom_num;
  *parent_atom_num = rand() % *parent_atom_num + 1;
  *child_atom_num = temp - *parent_atom_num;
}
