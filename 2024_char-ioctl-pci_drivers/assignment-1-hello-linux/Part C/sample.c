#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int guess;

int main()
{
	printf("Guess a number 0-9\n");
	scanf("%d", &guess);
	if ((rand()%10) != guess){
		printf("Nope!\n");
	}
	else{
		printf("Yup!\n");
	}
}
