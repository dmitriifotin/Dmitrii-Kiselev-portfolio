/**
 * question1A.c() - accepts a phrase from the console and checks if the phrase is a palindrome
 *
 * @author     Dmitrii Fotin (dfotin@pdx,edu)
 * @date       10.27.22
 *
 *  Reads lines of text from the console and checks if the input is a palindrome.
 *  Uses and increments pointers as opposed to ints for loops
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define MAXLINE 5000

char lineEntered[MAXLINE]; // full input
char lineEnteredAlphaOnly[MAXLINE]; // alpha chars only input
int letterCounter = 0; // counter keeping track of num of alpha chars
int palindrome = 0; // logic var tracking is input is palindrome or not 1/0

int main() {

    // greet the user and display the working directory for the application
    printf("Draw, o coward!\n");
    errno = 0;
    char *buf = getcwd(NULL, 0);    // allocates a buffer large enough to hold the path
    if (buf == NULL) {
        perror("getcwd");
        printf("Could not display the path\n");
    }
    else {
        printf("Current working directory: %s\n", buf);
        free(buf);
    }
    printf("\n");

    while(1) {
        // get the input and store the full phrase in the lineEntered and the alpha chars only in the lineEnteredAlphaOnly
        // so those alpha chars can be compared further down
        for (int i = 0; i < MAXLINE - 1; ++i) {
            lineEntered[i] = getchar();
            if (isalpha(lineEntered[i])) {
                lineEnteredAlphaOnly[letterCounter] = lineEntered[i];
                ++letterCounter;
            }
            if(lineEntered[i] == '\n') {
                --letterCounter;
                break;
            }
        }

        // check chars one by one from start and end if they match
        //Letter counter decrements from the end and i increments from the start
        for (int i = 0; i <= letterCounter; i++) {
            if (lineEnteredAlphaOnly[i] != lineEnteredAlphaOnly[letterCounter]) {
                palindrome = 0;
                break;
            }
            else {
                palindrome = 1;
            }
            --letterCounter;
        }

        // print if the phrase is palindrome or not
        if (palindrome == 1) {
            printf("\n%s\nPALINDROME\n\n", lineEntered);
        }
        else {
            printf("\n%s\nNOT A PALINDROME\n\n", lineEntered);
        }
        // clear all strings and vars for next run
        for (int i = 0; i < strlen(lineEntered); ++i) {
            lineEntered[i] = '\0';
        }
        for (int i = 0; i < strlen(lineEnteredAlphaOnly); ++i) {
            lineEnteredAlphaOnly[i] = '\0';
        }
        letterCounter = 0;
        palindrome = 0;
    }
}

