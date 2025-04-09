/**
 * STARTER CODE
 *
 * lookup_dialcode.c() - looks up a dialing country code for a country
 *
 * @author     Dmitrii Fotin (dfotin@pdx.edu)
 * @date       10/27/22
 *
 *  Uses an array of struct as a simple database to lookup a country code when given a text string
 *  containing a country.  Displays the country and its dialing country code if the country is
 *  in the table.  Prints an error message if it is not.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define MAXINPUT 20

// ADD YOUR #INCLUDES (IF ANY) HERE

/***** enums, constants, and typedefs *****/
typedef struct {
    char* country;
    int code;
} country_code_entry_t;

// ADD YOUR ENUMS, CONSTANTS, AND TYPEDEFS (IF ANY) HERE

/***** global variables *****/
const country_code_entry_t country_codes[] =
    {{"Argentina",              54},    {"Bangladesh",              880},
     {"Brazil",                 55},    {"Burma (Myanmar)",         95},
     {"China",                  86},    {"Columbia",                57},
     {"Congo, Dem. Rep. of",    243},   {"Egypt",                   20},
     {"Ethiopia",               251},   {"France",                  33},
     {"Germany",                49},    {"India",                   91},
     {"Indonesia",              62},    {"Iran",                    98},
     {"Italy",                  39},    {"Japan",                   81},
     {"Mexico",                 52},    {"Nigeria",                 234},
     {"Pakistan",               92},    {"Phillipines",             63},
     {"Poland",                 48},    {"Russia",                  7},
     {"South Africa",           27},    {"South Korea",             83},
     {"Spain",                  34},    {"Sudan",                   249},
     {"Thailand",               66},    {"Turkey",                  90},
     {"Ukraine",                380},   {"United Kingdom",          44},
     {"United States",          1},     {"Vietnam",                 84}};


char countryInput[MAXINPUT];

/***** Function prototypes *****/
// ADD YOUR FUNCTION PROTOYPES (IF ANY) HERE

/***** main() *****/
int main() {

    // ADD YOUR LOCAL VARIABLES HERE

    // Greet the user and display the working directory for the application
    // TODO:  Personalize the greeting message.  Note:  you need to be running
    // the gcc command line tools on a shell like bash.
    printf("Appelle mon numéro\n");
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

    // ADD YOUR CODE HERE
    while(1) {
        printf("Enter name of country:\n");
        fgets(countryInput, MAXINPUT, stdin);
        if (countryInput[0] == '\n') {
            printf("Good bye!\n");
            break;
        }
        countryInput[strlen(countryInput) - 1] = '\0';
        fflush(stdin);

        for (int i = 0; i < 32; ++i) {
            if (strcmp(countryInput,country_codes[i].country) == 0) {
                printf("The dialing code for %s is %d\n\n", country_codes[i].country, country_codes[i].code);
                break;
            }
            else if (i == 31){
                printf("Could not find a dialing code for %s\n\n", countryInput);
            }
        }
    }
 }
