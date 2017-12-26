#include <stdlib.h>
#include <stdio.h>
int main(int argc,char *argv[])
{
    int len=5;
    int i;
    int *pt=(int*)malloc(len*sizeof(int));
    int *p=pt;
    for(i=0;i<len;i++)
    {
        p++;
    }
    *p=5;
    return 0;
}
