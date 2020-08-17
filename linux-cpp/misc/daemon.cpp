#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

namespace ns_daemon1
{
    void main()
    {
        if (daemon(1, 0) == -1) // 貌似无法执行成功
        {
            std::cout << "daemon failed\n";
            return;
        }

        while (1)
        {
            sleep(1);
        }
    }
} // namespace ns_daemon1

namespace ns_daemon2
{
#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0);

    void creat_daemon(int nochdir, int noclose)
    {
        pid_t pid;
        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork error");
        if (pid > 0)
            exit(EXIT_SUCCESS);
        if (setsid() == -1)
            ERR_EXIT("SETSID ERROR");
        if (nochdir == 0)
            chdir("/");
        if (noclose == 0)
        {
            int i;
            for (i = 0; i < 3; ++i)
            {
                close(i);
                open("/dev/null", O_RDWR);
                dup(0);
                dup(0);
            }

            umask(0);
            return;
        }
    }

    void main()
    {
        creat_daemon(1, 0); // 貌似无法执行成功

        while (1)
        {
            sleep(1);
        }
    }
} // namespace ns_daemon2

int main()
{
    std::cout << "Hello, daemon\n";

    ns_daemon1::main();
    return 0;
}