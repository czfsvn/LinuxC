#include <stdlib.h>
#include <stdio.h>

int main(int argc,char *argv[])
{ 
    char *p=(char*)malloc(10);
    char *pt=p;
    int i;
    for(i=0;i<10;i++)
    {
        p[i]='z';
    }
    delete p;
    p[1]='a';
    free(pt);
    return 0;
}
