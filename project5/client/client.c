#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#include <dirent.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_PATH_SIZE 256

char** txtFilePaths;
int numOfFiles;

void recursiveTraverse(char* path)
{
    char recursiveDirectoryStorage[200][200];

    struct dirent* directoryPointer;
    DIR* mydir;

    int currentDirStorInt = 0;

    int excludePeriods = 0;

    if ((mydir = opendir(path)) == NULL) //error checking to see if the path is valid
    {
        perror("recursiveTraverse opendir");
        printf("Can't open %s\n", path);
        return;
    }

    while ((directoryPointer = readdir(mydir)) != NULL)
    {
        if (excludePeriods < 2) //checking for the undesireable outputs
        {
            excludePeriods++;
        }
        //checks if we want to recurse and if it is a folder to enter into
        else if (directoryPointer->d_type == 4)
        {
            sprintf(recursiveDirectoryStorage[currentDirStorInt], "%s%s", path, directoryPointer->d_name); //appends the path into the array
            currentDirStorInt++;
        }
        else //Outputs all file names
        {
            char* txtFilePath = (char*)malloc(sizeof(path) + sizeof(directoryPointer->d_name) + 2);
            sprintf(txtFilePath, "%s%s", path, directoryPointer->d_name);

            numOfFiles++;

            if (numOfFiles == 1)
            {
                txtFilePaths = (char**)malloc(sizeof(char**));
            }
            else if (numOfFiles > 1)
            {
                txtFilePaths = (char**)realloc(txtFilePaths, sizeof(char**) * numOfFiles);
            }

            txtFilePaths[numOfFiles - 1] = txtFilePath;
        }
    }

    for (int i = 0; i < currentDirStorInt; i++)
    {
        strcat(recursiveDirectoryStorage[i], "/");
        recursiveTraverse(recursiveDirectoryStorage[i]);
    }

    closedir(mydir); //closes files
}

int main(int argc, char* argv[])
{
    time_t current_time;
    char timeBuf[30];
    time(&current_time);
    strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Client starting...\n", timeBuf);

    //exits if arguments are not correct
    if (argc != 3)
    {
        printf("Wrong number of arguments.Exiting...\n");
        return 0;
    }

    numOfFiles = 0;

    char* dirname = argv[1];
    int numOfClients = atoi(argv[2]);

    if (numOfClients < 1 || numOfClients > 30) {
		printf("Incorrect number of clients. Enter between 1 and 30\n");
		exit(EXIT_FAILURE);
	}

    //converting dirname to absolute filepath
    char* absdirname = realpath(dirname, NULL);
    strcat(absdirname, "/");

    time(&current_time);
    strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Directory %s traversal and file partitioning\n", timeBuf, absdirname);

    //recursively look for txt files and add them to txtFilePaths
    recursiveTraverse(absdirname);


    //exit if no files were found
    if (numOfFiles == 0)
    {
        printf("No text files found. Exiting...\n");
        free(absdirname);
        exit(1);
    }

    //start partitioning the files to clients
    int baseFilesPerClient = numOfFiles / numOfClients;
    int bonusFiles = numOfFiles % numOfClients;

    //if directory does not exist, creates it
    struct stat dirstat;
    if (stat("ClientInput", &dirstat) == -1)
    {
        mkdir("ClientInput", 0777);
    }

    int i;
    int fileLineCounter = 0;

    for (i = 0; i < numOfClients; i++) //iterate through clients
    {
        int j;
        FILE* fp;
        char filename[30];

        //Creating the required filenames
        sprintf(filename, "ClientInput/Client%d.txt", i);
        fp = fopen(filename, "w");

        if (fp == NULL)
        {
            perror("ClientInput fopen");
            free(absdirname);
            exit(1);
        }

        for (j = 0; j < baseFilesPerClient; j++) //writes base number of file paths
        {
            fprintf(fp, "%s\n", txtFilePaths[fileLineCounter]);
            fileLineCounter++;
        }
        if (bonusFiles > 0) //adds an additional bonus file if needed
        {
            fprintf(fp, "%s\n", txtFilePaths[fileLineCounter]);
            fileLineCounter++;
            bonusFiles--;
        }
        fclose(fp);
    }

    key_t keyCtoS = 123;
    int msgQ = msgget(keyCtoS, 0666 | IPC_CREAT);

    pid_t pid;

    i = 0;
    do
    {
        if (i == 0 || pid > 0)
        {
            pid = fork();
            //error checking
            if (pid < 0)
            {
                perror("main forking");
                free(absdirname);
                exit(0);
            }
        }
        i++;
    } while (i < numOfClients && pid != 0); //only parent forks (c amount of times)

    //children operations
    if (pid == 0)
    {
        int clientID = i;
        struct msgbuf sendBuf;
        struct msgbuf recvBuf;
        sendBuf.mtype = clientID;

        char inputFileName[30];
        sprintf(inputFileName, "ClientInput/Client%d.txt", clientID - 1);
        FILE* inputFP = fopen(inputFileName, "r");

        if (inputFP == NULL)
        {
            perror("iClientInput fopen");
            free(absdirname);
            exit(1);
        }

        while (fgets(sendBuf.mtext, 256, inputFP) != NULL) //recieve file path in text file
        {
            sendBuf.mtext[strcspn(sendBuf.mtext, "\n")] = 0;
            //printf("Sending %s of type: %ld\n", sendBuf.mtext, sendBuf.mtype);
            time(&current_time);
            strcpy(timeBuf, ctime(&current_time));
            timeBuf[strcspn(timeBuf, "\n")] = 0;
            printf("[%s] Sending %s from client process %d\n", timeBuf, sendBuf.mtext, clientID - 1);
            //send file path
            if (-1 == msgsnd(msgQ, (void*)&sendBuf, sizeof(sendBuf), 0)) {

                perror("sending file");
                free(absdirname);
                exit(1);

            }
            //recieve ACK
            msgrcv(msgQ, (void*)&recvBuf, sizeof(recvBuf), clientID + 30, 0);

            if (-1 == msgrcv(msgQ, (void*)&recvBuf, sizeof(recvBuf), clientID + 30, 0)) {

                perror("rec ack");
                free(absdirname);
                exit(1);

            }

            time(&current_time);
            strcpy(timeBuf, ctime(&current_time));
            timeBuf[strcspn(timeBuf, "\n")] = 0;
            printf("[%s] Client process %d received ACK for %s\n", timeBuf, clientID - 1, sendBuf.mtext);
            if (strcmp(recvBuf.mtext, "ACK") != 0)
            {
                printf("Error: recieved %s instead of ACK. Continuing... \n", recvBuf.mtext);
            }
        }

        fclose(inputFP);

        strcpy(sendBuf.mtext, "END");
        //send END

        if (-1 == msgsnd(msgQ, (void*)&sendBuf, sizeof(sendBuf), 0)) {

            perror("sending end");
            free(absdirname);
            exit(1);

        }

        time(&current_time);
        strcpy(timeBuf, ctime(&current_time));
        timeBuf[strcspn(timeBuf, "\n")] = 0;
        printf("[%s] Sending END from client process %d\n", timeBuf, clientID - 1);

        //recieve global result
        
        if (-1 == msgrcv(msgQ, (void*)&recvBuf, sizeof(recvBuf), clientID + 30, 0)) {

            perror("rec result");
            free(absdirname);
            exit(1);

        }

        time(&current_time);
        strcpy(timeBuf, ctime(&current_time));
        timeBuf[strcspn(timeBuf, "\n")] = 0;
        printf("[%s] Client process %d received (%s) from server\n", timeBuf, clientID - 1, recvBuf.mtext);

        //if directory does not exist, creates it
        struct stat dirstat;
        if (stat("ClientOutput", &dirstat) == -1)
        {
            mkdir("ClientOutput", 0777);
        }

        FILE* outFP;
        char outputFileName[30];
        sprintf(outputFileName, "ClientOutput/Client%d.txt", clientID - 1);

        outFP = fopen(outputFileName, "w");
        fprintf(outFP, "%s\n", recvBuf.mtext);

        fclose(outFP);

        free(absdirname);

        int count;
        for (count = 0; count < numOfFiles; count++)
        {
            free(txtFilePaths[count]);
        }
        free(txtFilePaths);

        return 0;
    }
    else //parent operations
    {
        int counter;
        for (counter = 0; counter < numOfClients; counter++)
        {
            //wait for all children
            if (wait(NULL) < 0)
            {
                perror("ERROR: failed to wait for child");
                free(absdirname);
                int count;
                for (count = 0; count < numOfFiles; count++)
                {
                    free(txtFilePaths[count]);
                }
                free(txtFilePaths);
                exit(0);
            }
        }
    }

    time(&current_time);
    strcpy(timeBuf, ctime(&current_time));
    timeBuf[strcspn(timeBuf, "\n")] = 0;
    printf("[%s] Client has finished.\n", timeBuf);

    int count;
    for (count = 0; count < numOfFiles; count++)
    {
        free(txtFilePaths[count]);
    }
    free(txtFilePaths);


    //end clean up
    if (msgctl(msgQ, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        free(absdirname);
        exit(1);
    }
    free(absdirname);
    return 0;

}