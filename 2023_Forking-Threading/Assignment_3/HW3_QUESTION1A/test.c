#include <stdio.h>
#include <stdlib.h>

int *i;

int main() {
    i = (int *) malloc(sizeof(int));
    *i = 10;
    *i += 2;
    printf("%d", *i);
    return 0;
}
