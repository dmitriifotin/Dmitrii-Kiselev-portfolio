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
int Thread_count = DEFAULT_THREAD;
int found = 0;
int row, col;

int checkForMatch(int row, int col) {
  int r,c, length;
  int detect = 0;

  for(length=0, c=col; c < Cols; c++)
     if( Image[row][c] == 1 )
	{ if( ++length == Detect_len ) { detect++; break; } }
     else break;

  for(length=0, r=row; r < Rows; r++)
     if( Image[r][col] == 1 )
       { if( ++length == Detect_len ) { detect++; break; } }
     else break;

  return detect;
}

void makeAnImage() {
    for(row=0; row < Rows; row++)
        for(col=0; col< Cols; col++)
            Image[row][col] = rand() % 2;
}

void *ThreadSeek (void *vargp){
    for(row=0; row < Rows; row++)
        for(col=0; col < Cols; col++)
            found += checkForMatch(row,col);
    return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread_id;

  for( argc--, argv++; argc > 0; argc-=2, argv+=2  ) {
          if      (strcmp(argv[0], "-s" ) == 0 ) srand ( atoi(argv[1]) );
          else if (strcmp(argv[0], "-r" ) == 0 ) Rows = atoi(argv[1]);
          else if (strcmp(argv[0], "-c" ) == 0 ) Cols = atoi(argv[1]);
          else if (strcmp(argv[0], "-l" ) == 0 ) Detect_len = atoi(argv[1]);
          else if (strcmp(argv[0], "-t" ) == 0 ) {
                Thread_count = atoi(argv[1]);
                if (Thread_count != 1 && Thread_count != 2 && Thread_count != 4 && Thread_count != 8 && Thread_count != 16) {
                    printf("\nInvalid Number of Threads\n");
                    exit(-1);
                }
          }
          else { printf("\nInvalid Arguments\n"); exit(-1); }
      }

  makeAnImage();

  for (int i = 0; i < Thread_count; i++)
        pthread_create(&thread_id, NULL, ThreadSeek, (void *)&thread_id);

  printf("\nTOTAL DETECTED: %d\n", found);

  exit(0);
}
