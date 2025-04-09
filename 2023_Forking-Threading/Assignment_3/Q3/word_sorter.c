/**
 * STARTER CODE
 *
 * word_sorter.c() - sorts a list of words
 *
 * @author     Dmitrii Fotin (dfotin@pdx.edu)
 * @date       10/30/22
 *
 *  Uses malloc() and free() to allocate space for the words.  Words are stored
 *  in an array of pointers to the character strings containing the words.  Does
 *  the sort using the C library qsort() function.  Compare function is provided
 *  in the starter code.
 *
 *  NOTES:
 *  - does not check for illegal inputs, e.g. numbers and symbols
 *  - gets the whole line in, not just the first word, does not filter out words so only one word remains per string
 *
 *  Original problem from C Programming: A Modern Approach:2e by K.N. King.  Modified
 *  by Roy Kravitz to use getaline() along w/ a few other changes to make the program
 *  more readable
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/***** enums, constants, and typedefs *****/
#define MAX_WORDS 50
#define WORD_LEN 20

/***** global variables *****/
// ADD YOUR GLOBAL VARIABLES HERE

/***** Function prototypes *****/
int compare_strings(const void *p, const void *q);

/***** main() *****/
int main(void)
{
    char *words[MAX_WORDS];     // array of pointers to strings containing words
    int num_words = 0;              // number of words in the words[]
    char buffer[WORD_LEN];

    // ADD YOUR LOCAL VARIABLES HERE

    // Greet the user and display the working directory for the application
    // TODO:  Personalize the greeting message.  Note:  you need to be running
    // the gcc command line tools on a shell like bash.
    printf("Let's get this sorted!\n");
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
        printf("Enter word: ");
        fgets(buffer, WORD_LEN, stdin); // gets input into a buffer string
        // if string is just a new line, input is over and the loop is broken
        if(strlen(buffer) == 1) {
            break;
        }
        // allocate enough memory to the pointer based on the length of the buffer string
        words[num_words] = (char *) malloc(sizeof(char) * strlen(buffer));
        // copy the string to the pointer address
        strcpy(words[num_words], buffer);
        // clear input
        fflush(stdin);
        // increment the word counter
        ++num_words;
    }

    // user entered an empty string
    // all the words are in the array so sort them
    qsort(words, num_words, sizeof(char *), compare_strings);

    // display the sorted list
    printf("\nIn sorted order:");
    for (int i = 0; i < num_words; i++) {
        printf(" %s", words[i]);
    }
    printf("\n");

    printf("Exiting word sorting application\n");

    // free the memory allocated to the array of strings
    for(int i = 0; i <= num_words; ++i) {
        free(words[i]);
    }
    return 0;
}

/**
 * compare_strings() - compares to strings.  Used by qsort()
 *
 * @param   pointer to the first string
 * @param   pointer to the string to compare it to
 *
 * @returns 0 if the strings are equal, < or > to compare strings
 */
int compare_strings(const void *p, const void *q)
{
    return strcmp(*(char **)p, *(char **)q);
}
