#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>

#define MAX_PATH_SIZE 256

char** txtFilePaths;
int numOfFiles;

void recursiveTraverse(char *path)
{
  char recursiveDirectoryStorage[200][200];
	
  struct dirent *directoryPointer;
  DIR *mydir;

	int currentDirStorInt = 0;
	
	int excludePeriods = 0;

  if ((mydir = opendir(path)) == NULL) //error checking to see if the path is valid
	{
    perror("dude!");
		printf("Can't open %s\n", path);
      return;
  }
		
	//printf("In directory: %s\n", path); formatting
	
  while ((directoryPointer = readdir(mydir)) != NULL) 
	{
		if(excludePeriods < 2) //checking for the undesireable outputs
		{
			excludePeriods++;
		}
		//checks if we want to recurse and if it is a folder to enter into
		else if (directoryPointer->d_type == 4)
		{
      sprintf(recursiveDirectoryStorage[currentDirStorInt], "%s%s", path, directoryPointer->d_name); //appends the path into the array
      currentDirStorInt++;
			//printf(" %s ", directoryPointer->d_name);
    }
    else //Outputs all file names
		{
      //printf(" %s ", directoryPointer->d_name);

      char * txtFilePath = (char*) malloc(sizeof(path) + sizeof(directoryPointer->d_name) + 2);
      //printf("- \n%s \n-", path);
      sprintf(txtFilePath, "%s%s", path, directoryPointer->d_name);

      numOfFiles++;

      if (numOfFiles == 1)
      {
        txtFilePaths = (char**) malloc(sizeof(char**));
      }
      else if (numOfFiles > 1)
      {
        txtFilePaths = (char**) realloc(txtFilePaths, sizeof(char**) * numOfFiles);
      }

      txtFilePaths[numOfFiles - 1] = txtFilePath;
		}
	}
		
	//printf("\n"); formatting
		
	for (int i = 0; i < currentDirStorInt; i++)
	{
    strcat(recursiveDirectoryStorage[i], "/");
		recursiveTraverse(recursiveDirectoryStorage[i]);
	}

   closedir(mydir); //closes files
}

int main(int argc, char* argv[])
{

  if(argc != 3)
  {
      printf("Wrong number of arguments...exiting...\n");
      return 0;
  }

  numOfFiles = 0;

  char* dirname = argv[1];
  int numOfClients = atoi(argv[2]);

  recursiveTraverse(dirname);

  int i;
  for(i = 0; i < numOfFiles; i++)
  {
    printf("%s\n", txtFilePaths[i]);
  }
}