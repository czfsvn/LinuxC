#include <stdio.h>

using namespace std;

#define __u8    unsigned char
#define __u16   unsigned short


// __attribute__ ((packed)) 的位置约束是放于声明的尾部“；”之前

struct str_struct{
    __u8    a;
    __u8    b;
    __u8    c;
    __u16   d;
} __attribute__ ((packed));

/*  当用到typedef时，要特别注意__attribute__ ((packed))放置的位置，相当于：
*  typedef struct str_stuct str;
*  而struct str_struct 就是上面的那个结构。
*/
typedef struct {
    __u8    a;
    __u8    b;
    __u8    c;
    __u16   d;
} __attribute__ ((packed)) str;

/* 在下面这个typedef结构中，__attribute__ ((packed))放在结构名str_temp之后，其作用是被忽略的，注意与结构str的区别。*/
typedef struct {
    __u8    a;
    __u8    b;
    __u8    c;
    __u16   d;
}str_temp __attribute__((packed));

typedef struct {
    __u8    a;
    __u8    b;
    __u8    c;
    __u16   d;
}str_nopacked;


int main(void)
{
    printf("sizeof str = %d\n", sizeof(str));
    printf("sizeof str_struct = %d\n", sizeof(struct str_struct));
    printf("sizeof str_temp = %d\n", sizeof(str_temp));
    printf("sizeof str_nopacked = %d\n", sizeof(str_nopacked));
    return 0;
}

