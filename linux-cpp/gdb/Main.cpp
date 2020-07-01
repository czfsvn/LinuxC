#include <iostream>
#include <ucontext.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

/*
    可以在gdb时， p MAX(1, 4)   将 1， 4 的最大值打印出来
    需要在编译的时候 加上 g3： g++ Main.cpp -g3

    -g 默认选项，同-g2
    -g0 不生成任何调试信息，和编译时不加“-g”是一样的。
    -g1 生成最少量的调试信息，这些信息足够用来通过backtrace查看调用栈符号信息。主要包括了函数声明、外部变量和行号等信息，但是不包含局部变量信息。这个选项比较少用。
    -g2 生成足够多的调试信息，可以用GDB进行正常的程序调试，这个是默认选项。
    -g3 在-g2的基础上产生更多的信息，如宏定义。
*/
#define MAX(x, y) (x) > (y) ? (x) : (y)

namespace ns_test1
{
    /*
        可以动态打印变量：在GDB的时候打印变量的数值，
        
        格式：dprintf location,format string,arg1,arg2,...
        location可以是文件名:行号、函数名、或者具体的地址等。

        如：
        dprintf 24, "hello, main.cpp\n"
        dprintf 28, "a = %d, b = %d\n", a, b
    */
    /*   在gdb模式下保存断点信息： save breakpoints file_name
        加载方式：
            1)  gdb a.out -x file_name
            2)  在gdb里面使用： source file_name

   */
    void main()
    {
        int32_t i, a = 1, b = 2;

        a = 3;
        b = 4;
        for (i = 0; i < 5; i++)
        {
            a = a + i;
            b = a * 2;
        }
        std::cout << "a : " << a << ", b : " << b << std::endl;
    }
} // namespace ns_test1

namespace ns_test2
{
    struct TypeStruct
    {
        int a;
        char *str;
        int b;
    };
    void main()
    {
        TypeStruct type1;

        std::cout << "Hello, ns_test2::main\n";
    }
} // namespace ns_test2

namespace ns_test3
{
    /*
        b bubble_sort
        commands 1
            printf "The original value of n is %d\n",n
            set var n=10
            printf "Current value of n is %d\n",n
            continue
        end
    */
    void bubble_sort(int a[], int n)
    {
        int i = 0, j = 0, tmp;
        for (i = 0; i <= n; i++)
        {
            for (j = 0; j < n - i - 1; j++)
            {
                if (a[j] > a[j + 1])
                {
                    tmp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = tmp;
                }
            }
        }
    }

    void main()
    {
        int arr[10] = {64, 34, 25, 12, 22, 11, 90, 45, 86, 23};
        bubble_sort(arr, sizeof(arr));
        for (int i = 0; i < sizeof(arr); i++)
        {
            std::cout << arr[i] << ",";
        }
        std::cout << std::endl;
    }
} // namespace ns_test3

namespace ns_test4
{
    static void signal_handler1(int sig, siginfo_t *info, void *ctx)
    {
        ucontext_t *context = (ucontext_t *)ctx;
        printf("Signal = %d, Memory location = %p\n"
               "RIP = %016X, RSP = %016X, RBP = %016X\n"
               "RAX = %016X, RBX = %016X, RCX = %016X\n"
               "RDX = %016X, RSI = %016X, RDI = %016X\n"
               "R8 = %016X, R9 = %016X, R10 = %016X\n"
               "R11 = %016X, R12 = %016X, R13 = %016X\n"
               "R14 = %016X, R15 = %016X, RFLAGS = %016X\n\n",
               sig, info->si_addr,
               context->uc_mcontext.gregs[REG_RIP],
               context->uc_mcontext.gregs[REG_RSP],
               context->uc_mcontext.gregs[REG_RBP],
               context->uc_mcontext.gregs[REG_RAX],
               context->uc_mcontext.gregs[REG_RBX],
               context->uc_mcontext.gregs[REG_RCX],
               context->uc_mcontext.gregs[REG_RDX],
               context->uc_mcontext.gregs[REG_RDI],
               context->uc_mcontext.gregs[REG_R8],
               context->uc_mcontext.gregs[REG_R9],
               context->uc_mcontext.gregs[REG_R10],
               context->uc_mcontext.gregs[REG_R11],
               context->uc_mcontext.gregs[REG_R12],
               context->uc_mcontext.gregs[REG_R13],
               context->uc_mcontext.gregs[REG_R14],
               context->uc_mcontext.gregs[REG_R15],
               context->uc_mcontext.gregs[REG_EFL]);

        void *buf[256] = {0};
        int size = backtrace(buf, 256);
        backtrace_symbols_fd(buf, size, fileno(stdout));
        exit(-1);
    }

    /*
        此时想要打印堆栈信息的话， 需要root 权限来执行
        sudo ./a.out
    */
    static void signal_handler2(int sig, siginfo_t *info, void *ctx)
    {
        char cmd[256];
        printf("\n**** Segmentation fault happend, start gdb ....\n\n");

        snprintf(cmd, 256, "gdb --pid=%d -ex bt -q", getpid());
        system(cmd);

        printf("\n**** Finish debugging, now quit\n");
        exit(-1);
    }

    void test3(int *p)
    {
        *p = 1;
    }
    void test2(int *p)
    {
        test3(p);
    }
    void test1(int *p)
    {
        test2(p);
    }

    void main()
    {
        int *p = (int *)0x12345678;
        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_sigaction = signal_handler2;
        action.sa_flags = SA_SIGINFO;

        sigaction(SIGSEGV, &action, NULL);
        test1(p);
        return;
    }

} // namespace ns_test4

namespace ns_test5
{

    void test_little(void)
    {
        int i, j;

        for (i = 0; i < 30000000; i++)
            j = i;
    }

    void test_mdedium(void)
    {
        int i, j;

        for (i = 0; i < 60000000; i++)
            j = i;
    }

    void test_high(void)
    {
        int i, j;

        for (i = 0; i < 90000000; i++)
            j = i;
    }

    void test_hi(void)
    {
        int i, j;

        for (i = 0; i < 120000000; i++)
            j = i;
    }

    int main(void)
    {
        int i, pid, result;

        for (i = 0; i < 2; i++)
        {
            result = fork();
            if (result > 0)
                printf("i=%d parent parent=%d current=%d child=%d\n", i, getppid(), getpid(), result);
            else
                printf("i=%d child parent=%d current=%d\n", i, getppid(), getpid());

            if (i == 0)
            {
                test_little();
                sleep(1);
            }
            else
            {
                test_mdedium();
                sleep(1);
            }
        }

        pid = wait(NULL);
        test_high();
        printf("pid=%d wait=%d\n", getpid(), pid);
        sleep(1);
        pid = wait(NULL);
        test_hi();
        printf("pid=%d wait=%d\n", getpid(), pid);
        return 0;
    }
} // namespace ns_test5

namespace ns_test6
{
    void main()
    {
        uint32_t idx = 0;
        while (1)
        {
            std::cout << ++idx << std::endl;
            sleep(1);
        }
    }
} // namespace ns_test6

int main()
{
    std::cout << "Hello, World\n";
    //ns_test1::main();
    //ns_test2::main();
    //ns_test3::main();
    //ns_test4::main();
    //ns_test5::main();
    ns_test6::main();
    return 1;
}
