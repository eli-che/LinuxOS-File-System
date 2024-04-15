#include<stdio.h>

#include<stdlib.h>

#include<pthread.h>
#include <string.h>

#include<semaphore.h>

void * func(int n);
pthread_t philosopher[5];
pthread_mutex_t chopstick[5];
char* names[]={"X", "Lars", "Kurt", "Veselka", "Emilia", "Håkan"};

int main() {
  int i, k;
  void * msg;
  for (i = 1; i <= 5; i++) {
    k = pthread_mutex_init( & chopstick[i], NULL);
    if (k == -1) {
      printf("\n Mutex initialization failed");
      exit(1);
    }
  }
  for (i = 1; i <= 5; i++) {
    k = pthread_create( & philosopher[i], NULL, (void * ) func, (int * ) i);
    if (k != 0) {
      printf("\n Thread creation error\n");
      exit(1);
    }
  }
  for (i = 1; i <= 5; i++) {
    k = pthread_join(philosopher[i], & msg);
    if (k != 0) {
      printf("\n Thread join failed\n");
      exit(1);
    }
  }
  for (i = 1; i <= 5; i++) {
    k = pthread_mutex_destroy( & chopstick[i]);
    if (k != 0) {
      printf("\n Mutex Destroyed\n");
      exit(1);
    }
  }
  return 0;
}

void * func(int n) {
  printf("\n%s is thinking", *(names+n));
	sleep(10);
	if (n == 1){
		printf("\n%s took  the right chopstick", *(names+n));
		pthread_mutex_lock( & chopstick[5]); //Take right chopstick first.
		}
  pthread_mutex_lock( & chopstick[n]); //when philosopher 5 is eating he takes fork 1 and fork 5
	printf("\n%s took the left chopstick", *(names+n));
	printf("\n%s is thinking", *(names+n));
	sleep(1);
	if (n == 1){
				printf("\n%s is eating", *(names+n));
			}
	else {
		printf("\n%s attempts to take the right chopstick", *(names+n));
		pthread_mutex_lock( & chopstick[n-1]);
		printf("\n%s took the right chopstick", *(names+n));
  	printf("\n%s is eating", *(names+n));
	}
  sleep(20);
  pthread_mutex_unlock( & chopstick[n]);
	if (n == 1){pthread_mutex_unlock( & chopstick[5]);}
	else {pthread_mutex_unlock( & chopstick[n-1]);}
  printf("\n%s Finished eating", *(names+n));

}
