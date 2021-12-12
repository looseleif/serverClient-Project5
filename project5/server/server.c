#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

pthread_mutex_t letterLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t endLock = PTHREAD_MUTEX_INITIALIZER;

int globalArray[26] = { 0 };

int msgQ;
int numOfThreads;
int clientsFinished;

void updateGlobal(int array[26]) {


	pthread_mutex_lock(&letterLock);

	int i;

	for (i = 0; i < 26; i++) {

		globalArray[i] = globalArray[i] + array[i];

	}

	pthread_mutex_unlock(&letterLock);

}

void* process_client(void* param) {


	time_t current_time;
	char timeBuf[30];

	struct msgbuf sendMsg;
	struct msgbuf recvMsg;

	int thread_num = *((int*)param);

	int localArray[26] = { 0 };

	FILE* input;

	char c;

	int grabNext = 0;

	recvMsg.mtext[0] = 'A';

	//sends 'ACK'
	sendMsg.mtext[0] = 'A';
	sendMsg.mtext[1] = 'C';
	sendMsg.mtext[2] = 'K';

	int wordCount = 0;

	if( -1 == msgrcv(msgQ, (void*)&recvMsg, sizeof(recvMsg), thread_num, 0))
	{
		perror("receiving file paths1");
		exit(1);

	}
	time(&current_time);
	strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Thread %d recieved %s from client process %d\n", timeBuf, thread_num -1, recvMsg.mtext, thread_num - 1);
	//while we haven't received END
	do {

		//setting up message queue

		input = fopen(recvMsg.mtext, "r");

		//error checking
		if (input == NULL) {
			perror("Failed: ");
			return (void*)1;
		}

		//Getting our input for the first line
		c = fgetc(input);

		//checking for a capital letter
		if (c < 97) {
			c = c + 32;
		}

		//incrementing the correct character in the array
		localArray[c - 97]++;

		while (1) {

			//getting the character
			c = fgetc(input);

			//exits the while loop when we get to the end of the file
			if (feof(input)) {
				break;
			}

			//checks and increments the character into the array
			if (grabNext) {
				if (c < 97) {
					c = c + 32;
				}


				localArray[c - 97]++;
			}

			//if the character is at the end of the line, we increment grabNext to get the next line
			if (c == '\n') {
				grabNext = 1;
			}
			else {
				grabNext = 0;
			}
		}

		grabNext = 0;

		fclose(input);

		updateGlobal(localArray);

		int i;
		for (i = 0; i < 26; i++) {
			localArray[i] = 0;
		}

		sendMsg.mtype = thread_num + 30;

		//sends ACK
		if(-1 == msgsnd(msgQ, (void*)&sendMsg, sizeof(sendMsg), 0))
		{
			perror("sending ACK");
			exit(1);
		}
		time(&current_time);
		strcpy(timeBuf, ctime(&current_time));
    	timeBuf[strcspn(timeBuf, "\n")] = 0;
    	printf("[%s] Thread %d sending ACK to client %d for %s\n", timeBuf, thread_num -1, thread_num - 1, recvMsg.mtext);

		//receiving file paths
		if(-1 == msgrcv(msgQ, (void*)&recvMsg, sizeof(recvMsg), thread_num, 0))
		{
			perror("receiving file paths2");
			exit(1);
		}
		time(&current_time);
		strcpy(timeBuf, ctime(&current_time));
    	timeBuf[strcspn(timeBuf, "\n")] = 0;
    	printf("[%s] Thread %d recieved %s from client process %d\n", timeBuf, thread_num -1, recvMsg.mtext, thread_num - 1);

	} while (strcmp(recvMsg.mtext, "END") != 0);

	pthread_mutex_lock(&endLock);
	clientsFinished++;
	pthread_mutex_unlock(&endLock);

	//Waits until all clients are finished (sorry for wasting CPU cycles!)
	while (clientsFinished < numOfThreads) {;}

	char histogram[256];
	//Print final output
	int i;
	for (i = 0; i < 26; i++) {
		sprintf(histogram + strlen(histogram), "%d#", globalArray[i]);
	} 


	//send global result
	strcpy(sendMsg.mtext, histogram);
	if(-1 == msgsnd(msgQ, (void*)&sendMsg, sizeof(sendMsg), 0))
	{
		perror("sending result");
		exit(1);

	}

	time(&current_time);
	strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Thread %d sending final letter count to client process %d\n", timeBuf, thread_num -1, thread_num - 1);

	free(param);

	return 0x0;

}

int main(int argc, char* argv[])
{

	int threads;
	time_t current_time;
	char timeBuf[30];
	time(&current_time);
	strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Server is starting...\n", timeBuf);

	if (argc != 2) {
		printf("Incorrect arg count. Enter thread count\n");
		exit(EXIT_FAILURE);
	}

	threads = atoi(argv[1]);
	numOfThreads = threads;
	clientsFinished = 0;

	if (threads < 1 || threads > 30) {
		printf("Incorrect number of threads. Enter between 1 and 30\n");
		exit(EXIT_FAILURE);
	}

	key_t keyCtoS = 123;

	msgQ = msgget(keyCtoS, 0666 | IPC_CREAT);

	//end clean up
	if (msgctl(msgQ, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(1);
	}

	msgQ = msgget(keyCtoS, 0666 | IPC_CREAT);


	pthread_t threadArray[threads];

	int i;

	//Create the desired amount of threads
	for (i = 0; i < threads; i++)
	{

		int* clientNum = (int*)malloc(sizeof(*clientNum));

		*clientNum = i + 1;

		pthread_create(&threadArray[i], NULL, process_client, (void*)clientNum);

	}

	// Wait for threads
	for (i = 0; i < threads; i++)
	{
		pthread_join(threadArray[i], NULL);
	}

	time(&current_time);
	strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Server has finished.\n", timeBuf);

	return 0;
}