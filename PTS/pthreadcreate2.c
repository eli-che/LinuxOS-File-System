#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#define MAX 9999

struct threadArgs {
	unsigned int id;
	unsigned int numThreads;
	double squaredId;
};

void* child(void* params) {
	struct threadArgs *args = (struct threadArgs*) params;
	unsigned int childID = args->id;
	unsigned int numThreads = args->numThreads;
	args->squaredId = sqrt(childID);
	printf("Greetings from child #%u of %u\n", childID, numThreads);
	//free(args);
}

int main(int argc, char** argv) {
	pthread_t* children; // dynamic array of child threads
	struct threadArgs* args; // argument buffer
	unsigned int numThreads = 0;
	struct threadArgs arr[MAX];
	// get desired # of threads
	if (argc > 1)
	numThreads = atoi(argv[1]);
	children = malloc(numThreads * sizeof(pthread_t)); // allocate array of handles
	double array[10];
	for (unsigned int id = 0; id < numThreads; id++) {
		// create threads
	//	args = malloc(sizeof(struct threadArgs));
		arr[id].id = id;
		arr[id].numThreads = numThreads;
		pthread_create(&(children[id]), // our handle for the child
			NULL, // attributes of the child
			child, // the function it should run
			&arr[id]); // args to that function
	}
	printf("I am the parent (main) thread.\n");
	for (unsigned int id = 0; id < numThreads; id++) {
		pthread_join(children[id], NULL );
	}
	for (unsigned int id = 0; id < numThreads; id++) {
		printf("%lf\n", arr[id].squaredId);
	}

	free(children); // deallocate array
	return 0;
}
