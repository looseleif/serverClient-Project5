#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

pthread_mutex_t letterLock = PTHREAD_MUTEX_INITIALIZER;

int globalArray[26] = { 0 };
//int globalArray[26] = { [0 ... 25] = 0 };

int msgQ;

void updateGlobal(int array[26]) {


	pthread_mutex_lock(&letterLock);

	int i;

	for (i = 0; i < 26; i++) {

		globalArray[i] = globalArray[i] + array[i];

	}

	pthread_mutex_unlock(&letterLock);

}

void* process_client(void* param) {

	// REMEMBER TO FREE clientNum

	struct msgbuf sendMsg;

	struct msgbuf recvMsg;

	int thread_num = *((int*)param);

	int localArray[26] = { 0 };

	//printf("testing with thread_num: %d\n", thread_num);

	FILE* input;

	char c;

	int grabNext = 0;

	recvMsg.mtext[0] = 'A';

	// -----------------

	while (strcmp(recvMsg.mtext, "END") != 0) {

		//msgrcv(msgQ, (void*)&recvMsg, sizeof(recvMsg), thread_num, 0);

		input = fopen("txt.txt", "r");

		if (input == NULL) {
			perror("Failed: ");
			return (void*)1;
		}

		c = fgetc(input);

		if (c < 97) {
			printf("-2-");
			c = c + 32;
		}

		localArray[c - 97]++;

		while (1) {

			c = fgetc(input);

			if (feof(input)) {

				break;

			}

			if (grabNext) {

				if (c < 97) {

					c = c + 32;

				}

				localArray[c - 97]++;

			}

			if (c == '\n') {

				grabNext = 1;

			}
			else {

				grabNext = 0;

			}

		}

		fclose(input);

		updateGlobal(localArray);

		sendMsg.mtype = thread_num + 30;

		sendMsg.mtext[0] = 'A';
		sendMsg.mtext[1] = 'C';
		sendMsg.mtext[2] = 'K';

		//msgsnd(msgQ, (void*)&sendMsg, sizeof(sendMsg), 0);]

		recvMsg.mtext[0] = 'E';
		recvMsg.mtext[1] = 'N';
		recvMsg.mtext[2] = 'D';

		//printf("inside\n");

	}

	//printf("outside\n");


	free(param);

	return 0x0;

}

int main(int argc, char* argv[])
{

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

	msgQ = msgget(keyCtoS, 0666 | IPC_CREAT);


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

	for (i = 0; i < 26; i++) {

		printf("%d#", globalArray[i]);

	}

	printf("\n");

	return 0;
}