/*test machine: CSELAB_machine_name
* date: 12/12/2021
* name: Michael Johnson, Chase Anderson, Emir Sahbegovic
* x500: Joh18255, And08479, Sahbe001
*/


Purpose of the Program:
	The purpose of our program is to create a server to client communication program  that utilizes
	a message queue to efficiently send data back and forth. To proficiently demonstrate the 
	capabilities of the program, we designed it to use multithreading, multiprocessing and a message queue
	to read files and count up the first character of every word in the folder.

How to Compile the Program:

	Once one has correctly navigated to the source file, run the command "make" in a linux system,
	this will utilize gcc to compile and generate an output file.


What Our Program Does:
	Our program will use two files of code running at the same time in different terminals
	to use a message queue to transfer data between them. These different files of code are called 
	'server' and 'client'. The server will take 1 input from the user which is the amount client processes 
	that will be running. The client will take 2 inputs from the user, which is the file path and the amount of
	client processes as well. The client will use the number of client processes inputted to use multiprocessing
	to find file paths and send them to the server through a message queue. The server will receive these 
	paths and read each file and count each first letter of every word in the file. The server will tally 
	up all of the characters and put it in a local array. After it finishes reading the file, the server 
	will send an acknowledgement string to the client and the client will send the next file path. Once
	the client sends all of the file paths it stored, it will send an end string to the server and continue
	running until all clients are done. Finally, once the server has gotten all the end strings, it will
	total all of the created local arrays and add it up into one final array. This array will be sent to the
	client to be outputted into output files.
	
External Assumptions:

	1. We assume that the inputted number of desired client processes have a maximum of 30
	2. We assume that the words in the text files will be stricly ASCII letters

Our Team:

	Michael Johnson - Joh18255
	Chase Anderson - And08479
	Emir Sahbegovic - Sahbe001

Our Contributions: 
	
	Michael - README, error checking
	Chase - Server
	Emir - Client