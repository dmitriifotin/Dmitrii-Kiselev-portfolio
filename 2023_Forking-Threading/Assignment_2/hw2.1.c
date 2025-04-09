#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//set the flags
int setexit=0;

//Q: what does this function do?
//A: it checks for Ctrl-C interrupt to exit the infinite while loop
void sigintHandler(int sig_num)
{
	setexit = 1;
	fflush(stdout);
	exit(0);
}


int main()
{
	char input [500];
	char output [500];
	FILE* file = NULL;

    signal(SIGINT, sigintHandler);

    //Open the file for writing/appending data
    //Code here
    file = fopen("file.txt", "w"); //create the file if it doesn't exist
    fclose(file);
	file = fopen("file.txt", "a+"); //open the file again in append/read mode

	//Check for errors in file open; close the file and exit on error
    //code here
    if (file == NULL) {
        printf("Could not open file.txt.\n");
		fclose(file);
    }

//continuous loop to read and write
    while(1)
	{
	//why do we need this?
	//A: if Ctrl-C is pressed, the function sigintHandler sets setexit to 1
	//and then we break out of the loop/terminate the program
    	if(setexit) {
		break;
	}

    	printf("Enter string, Cntrl-C to exit \n");

	//Read the input from terminal
	//code here
        fflush(stdin);
    	fgets(input, 500, stdin);
        fflush(stdin);
        input[strlen(input)-1] = '\0';
        printf("%s\n", input);
        printf("File write was successful\n");
	}
    fclose(file); // need to exit the program with Ctrl-C to save the inputs to the file

    return 0;
}
