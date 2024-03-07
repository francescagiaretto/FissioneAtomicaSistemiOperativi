#include "library.h"

void generate_n_atom(int *, int *);

int main(int argc, char* argv[]){
    
    int parent_atom_num = atoi(argv[1]); int child_atom_num, en_lib, shmid;
    data_buffer * shmem_p;
    char division_atom_num[3], division_parent_num[4];
    
    // child accessing shmem
    shmid = atoi(argv[2]);
    shmem_p = (data_buffer *) shmat(shmid, NULL, 0);
    if (shmem_p == (void *) -1) {
        perror("Pointer not attached."); exit(EXIT_FAILURE);
    }

    if (parent_atom_num <= MIN_N_ATOMICO) { // checking if pid is less than minimum
        shmem_p -> data[0] = shmem_p -> data[0] + 1;
        kill(getpid(), SIGTERM);
    } 

    srand(getpid()); //*  getpid is a better option than time(NULL): time randomizes based on program time which may be
                    //*  identical for more than one atom, while pid is always different

    generate_n_atom(&parent_atom_num, &child_atom_num);

    // TODO gestire la fork quando lo richiede l'attivatore.

    sprintf(division_atom_num, "%d", child_atom_num);
    char * vec_atomo[] = {"atomo", division_atom_num, NULL};

    // TODO funzione energy() che incrementa l'energia liberata nelle statistiche del master
        switch (fork())
        {
            case -1:
                char * message = "meltdown.";
                termination(message, shmem_p, shmid);
            break;
            
            case 0: // checking child
                shmdt(shmem_p);
                if(execve("atomo", vec_atomo, NULL)==-1) {perror("Execve grandchild"); exit(EXIT_FAILURE);} 
            break;
            
            default: // checking parent
                en_lib = child_atom_num*parent_atom_num - MAX(child_atom_num, parent_atom_num);    
                shmem_p -> data[1] = shmem_p -> data[1] + en_lib; // saving relative produced energy in shmem

                sprintf(division_parent_num, "%d", parent_atom_num);
                char * new_vec_atomo[] = {"atomo", division_parent_num, NULL};
                en_lib = 0;
                if(execve("atomo", new_vec_atomo, NULL) == -1) {perror("Execve child"); exit(EXIT_FAILURE);} 
            break;
        }
}

// calculates atomic number after division
void generate_n_atom(int * parent_atom_num, int * child_atom_num) {
    int temp = *parent_atom_num;
    *parent_atom_num = rand()% *parent_atom_num + 1;
    *child_atom_num = temp - *parent_atom_num;

}