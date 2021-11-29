/*test machine: CSELAB_machine_name
* date: 11/21/2021
* name: Michael Johnson, Chase Anderson, Emir Sahbegovic
* x500: Joh18255, And08479, Sahbe001
*/


Purpose of the Program:
	The purpose of our program is to create a program that takes advantage of the 
	multithreading options available to us to create and traverse through a linked list.
	Our program will accomplish this by using the pthread commands to create threads
	that access and insert lines from the input text file and will correctly synchronize
	the variables to ensure 100% accuracy.
	
How to Compile the Program:

	Once one has correctly navigated to the source file, run the command "make" in a linux system,
	this will utilize gcc to compile and generate an output file.


What Our Program Does:
	Our program will take 2 inputs from the user, the text file and the amount of threads
	that will be used. It will then create the specified amount of threads and will
	concurrently read the text file line by line. The threads will then input the
	line number, and the content of the text file into a linked list node structure. 
	The program also records when the node is created and inserted into the linked list.
	When the nodes are being inserted into the linked list, the nodes will automatically 
	sort themselves based on which line the content was on in the text file. Finally,
	our program will sequentially print the linked list and print it out.



External Assumptions:

	1. We assume that the space is counted as an ASCII text and will therefore count as 
		a standalone line in the linked list.
	2. We assume traversal to be called only one time. We believe this to be true because 
		there is only one printout of the linked list contents in the project pdf.

Our Team:

	Michael Johnson - Joh18255
	Chase Anderson - And08479
	Emir Sahbegovic - Sahbe001

Our Contributions: 
	
	Michael - create_node, README
	Chase - process file, error checking, comments
	Emir - read_line, insert, traversal