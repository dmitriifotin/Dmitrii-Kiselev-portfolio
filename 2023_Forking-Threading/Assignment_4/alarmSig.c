#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int setExit=0;

//when alarm triggered, set to exit the while loop and print error message
void sigAlarmHandler(int sig)
{
	setExit = 1;
	fprintf(stderr, "\nExiting due to inactivity\n");
	exit(0);
}

int main()
{
	char input [500]; //string to store input
	signal(SIGALRM, sigAlarmHandler); //alarm catcher

    while(!setExit)
	{
		alarm(10); //set a 10s alarm
		printf("Enter string\n");
        //get input
       	fflush(stdin);
   		fgets(input, 500, stdin);
       	fflush(stdin);
       	input[strlen(input)-1] = '\0';
       	//print input
       	printf("You enetered: %s\n", input);
	}

    return 0;
}
