#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_ROWS     16000
#define MAX_COLS     16000
#define DETECT_LEN      20
#define DEFAULT_THREAD  1

int Rows        = MAX_ROWS;
int Cols        = MAX_COLS;
int Detect_len  = DETECT_LEN;
int Image[MAX_ROWS][MAX_COLS];
int Thread_count = DEFAULT_THREAD; //num of threads
int found = 0; //total counter
int row, col; //have to make these global

pthread_mutex_t lock; //initialize lock

// didn't change this at all
int checkForMatch(int row, int col) {
	int r,c, length;
	int detect = 0;

	for(length=0, c=col; c < Cols; c++)
		if( Image[row][c] == 1 ) {
			if( ++length == Detect_len ) {
				detect++;
				break;
			}
		}
	else break;

	for(length=0, r=row; r < Rows; r++)
		if( Image[r][col] == 1 ) {
			if( ++length == Detect_len ) {
				detect++;
				break;
			}
		}
	else break;

	return detect;
}
// didn't change this at all
void makeAnImage() {
    for(row=0; row < Rows; row++) {
        for(col=0; col< Cols; col++) {
            Image[row][col] = rand() % 2;
		}
	}
}
// new function that each thread executes
void *ThreadSeek (void *vargp){
	static __thread int found_in_thread = 0;    //declare local variable to keep count of occurrences each thread found
                                                //so threads don't  lock the total counter each time they found a new occurrence
	//here we set the row to where each thread should start (thread index * total rows / num of threads)
	//even if the number of threads exceeds the number of rows, the total number of occurrences found is still correct
    for(int trow = *(int*)vargp * Rows/Thread_count; trow < (*(int*)vargp + 1) * Rows/Thread_count; trow++) {
        for(int tcol = 0; tcol < Cols; tcol++){
            found_in_thread += checkForMatch(trow,tcol); //increment the local thread variable, which doesn't have to be locked
		}
	}
	printf("Thread %d found %d occurrences\n", *(int*)vargp + 1, found_in_thread);
	pthread_mutex_lock(&lock); //lock the total counter
	found += found_in_thread; //increment it by the total num of occurrences found by this thread
	pthread_mutex_unlock(&lock); //unlock it
    return NULL;
}

int main(int argc, char *argv[]) {
	for( argc--, argv++; argc > 0; argc-=2, argv+=2  ) {
		if      (strcmp(argv[0], "-s" ) == 0 ) srand ( atoi(argv[1]) );
		else if (strcmp(argv[0], "-r" ) == 0 ) Rows = atoi(argv[1]);
		else if (strcmp(argv[0], "-c" ) == 0 ) Cols = atoi(argv[1]);
		else if (strcmp(argv[0], "-l" ) == 0 ) Detect_len = atoi(argv[1]);
		//added the num of threads input and invalid input checking thread num is not 1/2/4/8/16
		else if (strcmp(argv[0], "-t" ) == 0 ) {
			Thread_count = atoi(argv[1]);
			if (Thread_count != 1 && Thread_count != 2 && Thread_count != 4 && Thread_count != 8 && Thread_count != 16) {
				printf("\nInvalid Number of Threads\n");
				exit(-1);
			}
		}
		else {
			printf("\nInvalid Arguments\n");
			exit(-1);
		}
	}

	makeAnImage();
    //declare the threads based on the thread input (or default 1)
	pthread_t threads[Thread_count];
	int thread_args[Thread_count]; //declare a thread args array with the same number of entries
	for (int i = 0; i < Thread_count; i++) {
		thread_args[i] = i; //set the thread number as a thread argument
		pthread_create(&threads[i], NULL, ThreadSeek, (void *)&thread_args[i]); //create a thread and pass its counter to execute the seek function
	}
	for (int i = 0; i < Thread_count; i++) {
		pthread_join(threads[i], NULL); //wait for all threads to terminate
	}
	printf("\nTOTAL DETECTED: %d\n", found);
	pthread_mutex_destroy(&lock); //destroy the lock
	exit(0);
}
