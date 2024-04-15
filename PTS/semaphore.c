#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int argc, char **argv)
{
	pid_t pid;
	int i, sem_id;
	int j = 0;
	union semun {
		int val; /* Value for SETVAL */
		struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET (defined in <sys/mem.h> )*/
		unsigned short *array; /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
	} arg;
	struct sembuf sem_ops;

	sem_id = semget(IPC_PRIVATE, 2, 0x03ff);
	arg.val = 1;
	semctl(sem_id, 0, SETVAL, arg);
	arg.val = 0;
	semctl(sem_id, 1, SETVAL, arg);
	pid = fork();

	if (pid) {
		for (i = 0; i < 100; i++) {
			sem_ops.sem_num = 1;
			sem_ops.sem_op = -1;
			sem_ops.sem_flg = 0;
			semop(sem_id, &sem_ops, 1);
			if(j > 9){ fflush(stdout); j = 0;}
			putchar('A'); j++;
			sem_ops.sem_num = 0;
			sem_ops.sem_op = 1;
			sem_ops.sem_flg = 0;
			semop(sem_id, &sem_ops, 1);
		}
	} else {
		for (i = 0; i < 100; i++) {
			sem_ops.sem_num = 0;
			sem_ops.sem_op = -1;
			sem_ops.sem_flg = 0;
			semop(sem_id, &sem_ops, 1);
			if(j > 9){ fflush(stdout); j = 0;}
			putchar('B'); j++;
			sem_ops.sem_num = 1;
			sem_ops.sem_op = 1;
			sem_ops.sem_flg = 0;
			semop(sem_id, &sem_ops, 1);
		}
		semctl(sem_id, 1, IPC_RMID);
	}
}
