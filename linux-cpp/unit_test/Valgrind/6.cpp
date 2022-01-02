#include <stdlib.h>

int main()
{
    int i, *x;
    x = ( int* )malloc(10 * sizeof(int));
    for (i = 0; i < 10; i++)
        x[i] = i;
    free(x);
}
