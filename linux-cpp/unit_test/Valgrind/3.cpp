#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char *argv[])
{ 
    char x[40];
    for(int i=0;i<40;i++)
    {
        x[i]=i;
    }
    strncpy(x+20,x,20); //Good
    strncpy(x+20,x,21); //Overlap
    x[39]='\0';
    strcpy(x,x+20); //Good
    x[39]=40;
    x[40]='\0';

    strcpy(x,x+20); //Overlap
    return 0;
}
