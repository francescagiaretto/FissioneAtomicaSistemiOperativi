#include "library.h"

int main() {

    // STRUCT per definire una sleep contata in nanosecondi, non possibile usando una sleep() normale.
    struct timespec step_nanosec;
    step_nanosec.tv_sec = 0;            
    step_nanosec.tv_nsec = STEP_ALIMENTATORE;
    nanosleep(&step_nanosec, NULL);
}