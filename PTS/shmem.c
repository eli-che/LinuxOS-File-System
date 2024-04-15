#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

int main(int argc, char **argv)
{
	struct shm_struct {
		unsigned number[10];
		unsigned empty;
	};
	volatile struct shm_struct *shmp;
	char *addr = NULL;
	pid_t pid;
	int i, var1, var2, shmid;
	struct shmid_ds *shm_buf;

	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
	shmp->empty = 0; var1 = 0; var2 = 0;
	pid = fork();
	if (pid != 0) {
		/* here's daddy */
		while (var1 < 100) {
			/* write to shmem */
			while (shmp->empty == 1); // IS BUFFER EMPTY? If we don't check we may drop numbers
			{
				for(int i = 0; i < 10; i++){
					var1++;
					shmp->number[i] = var1; // INSERT NUMBER TO BUFFER
				}
			}
			shmp->empty = 1; // SET BUFFER TO EMPTY
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child */
		while (var2 < 100) {
			/* read from shmem */
			while (shmp->empty == 0); // IS THERE SOMETHING IN BUFFER? We might read twice if not checked.
			{
				for(int i = 0; i < 10; i++){
					var2 = shmp->number[i];	 //WE TAKE NUMBERS
					printf("%d\n", var2);
				}
			}
			 //WE TAKE NUMBERS
			shmp->empty = 0; //SET BUFFER TO FULL
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
}
