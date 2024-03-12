#include "library.h"

void generate_n_atom(int *, int *);
int semid;


int main(int argc, char* argv[]){
	semid = semget(atoi(argv[3]), 0, IPC_CREAT | 0666);
	semctl(semid, 0, SETVAL, 1);

	sem.sem_num = WAITSEM;
	sem.sem_op = 0;
	semop(semid, &sem, 0); 
	printf("\n\n\nTEST ATOMO\n\n\n");
    
	int parent_atom_num = atoi(argv[1]); int child_atom_num, en_lib, shmid;
	int key = atoi(argv[2]);
	data_buffer * shmem_p;
	char division_atom_num[3], division_parent_num[4];

	// printf("\n%d, atomico = [%d]\n", getpid(), parent_atom_num);
	
	// child accessing shmem
	shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
	if (shmid == -1) {
		perror("Shared memory creation.\n"); exit(EXIT_FAILURE);
	}

	shmem_p = (data_buffer *) shmat(key, NULL, 0);
	if (shmem_p == (void *) -1) {
		perror("Pointer atomo not attached."); exit(EXIT_FAILURE);
	}

	if (parent_atom_num <= MIN_N_ATOMICO) { 
		shmem_p -> waste_rel = shmem_p -> waste_rel +1;
		kill(getpid(), SIGTERM);
	} 

	srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be
									//*  identical for more than one atom, while pid is always different

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
				if(execve("atomo", vec_atomo, NULL)==-1) {perror("Execve grandchild"); exit(EXIT_FAILURE);} 
			break;
			
			default: // checking parent
				en_lib = energy(child_atom_num, parent_atom_num);
				shmem_p -> prod_en_rel = shmem_p -> prod_en_rel + en_lib; // saving relative produced energy in shmem

				sprintf(division_parent_num, "%d", parent_atom_num);
				char * new_vec_atomo[] = {"atomo", division_parent_num, argv[2], NULL};
				if(execve("atomo", new_vec_atomo, NULL) == -1) {perror("Execve child"); exit(EXIT_FAILURE);} 
			break;
		}
}


void generate_n_atom(int * parent_atom_num, int * child_atom_num) {
  int temp = *parent_atom_num;
  *parent_atom_num = rand() % *parent_atom_num + 1;
  *child_atom_num = temp - *parent_atom_num;
}
