#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

pthread_mutex_t leterLock = PTHREAD_MUTEX_INITIALIZER;

int globalArray[26] = {0};
//int globalArray[26] = { [0 ... 25] = 0 };

int midCtoS;
int midStoC;

void* process_client(void* param) {

	// REMEMBER TO FREE clientNum

	struct msgbuf recvMsg;

	int thread_num = *((int*)param);

	printf("testing with thread_num: %d\n", thread_num);

	// -----------------

	while (1) {


		msgrcv(midCtoS, (void*)&recvMsg, sizeof(recvMsg), thread_num, 0);

		// read file

		// send ack

		perror("LOL");

		printf("Received: %s", buf.mtext);

	}

	// -----------------


	free(param);

	return 0x0;

}


int main(int argc, char* argv[])
{

	for (int i = 0; i < 26; i++) {

		printf("%d %d\n", globalArray[i],i);

	}

	int threads;

	if (argc != 2) {
		printf("Incorrect arg count. Enter thread count\n");
		exit(EXIT_FAILURE);
	}

	threads = atoi(argv[1]);

	if (threads < 1 || threads > 16) {
		printf("Incorrect number of threads. Enter between 1 and 16\n");
		exit(EXIT_FAILURE);
	}

	key_t keyCtoS = 101;

	midCtoS = msgget(keyCtoS, 0666 | IPC_CREAT);

	key_t keyStoC = 808;

	midStoC = msgget(keyStoC, 0666 | IPC_CREAT);

	pthread_t threadArray[threads];

	int i;

	for (i = 0; i < threads; i++)
	{
		
		int* clientNum = (int*)malloc(sizeof(*clientNum));

		*clientNum = i;

		pthread_create(&threadArray[i], NULL, process_client, (void*)clientNum);

	}

	for (i = 0; i < threads; i++)
	{
		pthread_join(threadArray[i], NULL);
	}

	printf("\n");

	return 0;
}