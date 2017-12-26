#include <stdlib.h>
int main()
{
    char *x = (char*)malloc(20);
    char *y = (char*)malloc(20);
    x=y;
    free(x);
    free(y);
    return 0;
}
