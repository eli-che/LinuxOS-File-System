#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid;
    pid_t pid2;
    unsigned i;
    pid_t ppid = getpid();
    unsigned niterations = 1000;
    pid = fork();
    if (pid != 0)
    printf("Child: %d\n", pid);
    if (pid != 0){
      pid2 = fork();
        if (pid2 != 0)
        printf("Child: %d\n", pid2);
    }
//    printf("WHAT: %d <--:", getpid());
    if (getpid() == ppid + 1) {
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
     }
     else if (getpid() == ppid + 2){
       for (i = 0; i < niterations; ++i)
           printf("C = %d, ", i);
     }
     else {
        for (i = 0; i < niterations; ++i)
            printf("B = %d, ", i);
    }
    printf("\n");
}
