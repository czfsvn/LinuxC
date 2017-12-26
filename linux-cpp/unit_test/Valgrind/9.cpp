#include <stdlib.h>
#include <stdio.h>
int main(void)
{
    char *ptr = malloc(10);
    ptr[12] = 'a'; // 内存越界
    memcpy(ptr +1, ptr, 5); // 踩内存
    char a[10];
    a[12] = 'i'; // 数组越界
    free(ptr); // 重复释放
    free(ptr);
    char *p1;
    *p1 = '1'; // 非法指针

    return 0;
}
