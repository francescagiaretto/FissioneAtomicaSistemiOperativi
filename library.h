#ifndef LIBRARY_H
#define LIBRARY_H

#define _GNU_SOURCE
#include "parameters.txt"
#include "library.c"

int send_atom_pid(int msgid, int pid);

int receive_pid(int msgid);

#endif /*LIBRARY_H*/