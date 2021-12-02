#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t leterLock = PTHREAD_MUTEX_INITIALIZER;

void* process_client(void* param) {




}


int main(int argc, char* argv[])
{

	int threads;

	int* clientNum = (int*) malloc(sizeof(threads);

	if (argc != 2) {
		printf("Incorrect arg count. Enter thread count\n");
		exit(EXIT_FAILURE);
	}

	threads = atoi(argv[2]);

	if (threads < 1 || threads > 16) {
		printf("Incorrect number of threads. Enter between 1 and 16\n");
		exit(EXIT_FAILURE);
	}


	int i;

	pthread_t threadArray[threads];

	for (i = 0; i < threads; i++)
	{
		pthread_create(&threadArray[i], NULL, process_client, (void*)clientNum);
	}

	for (i = 0; i < threads; i++)
	{
		pthread_join(threadArray[i], NULL);
	}


	printf("\n");

	free(filename);
	return 0;
}