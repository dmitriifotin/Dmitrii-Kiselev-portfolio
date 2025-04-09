#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

time_t timer = 0; //timer variable

int main(int argc,char* argv[])
{
    timer = time(NULL); //start timer
    int rc = fork();
    //check if fork fails
    if (rc < 0) {
        printf("Fork failed\n");
    }
    //if not, run the command entered with its arguments
    else if (rc == 0) {
	char *myargs[argc-1];
    	for (int i = 0; i < argc; ++i) {
        	myargs[i] = argv[i+1];
    	}

        execvp(myargs[0], myargs);

    }
    //once the child process terminates, calculate the time passed
    else {
	wait(NULL);
    	timer = time(NULL) - timer;
    	printf("Time passed: %d\n", timer);
    }

    return 0;
}


