#include <iostream>

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

using namespace std;

namespace ns_test1
{

    /* Read all available inotify events from the file descriptor 'fd'.
    wd is the table of watch descriptors for the directories in argv.
    argc is the length of wd and argv.
    argv is the list of watched directories.
    Entry 0 of wd and argv is unused. */

    static void handle_events(int fd, int *wd, int argc, char *argv[])
    {
        /* Some systems cannot read integer variables if they are not
        properly aligned. On other systems, incorrect alignment may
        decrease performance. Hence, the buffer used for reading from
        the inotify file descriptor should have the same alignment as
        struct inotify_event. */

        char buf[4096]
            __attribute__((aligned(__alignof__(struct inotify_event))));
        const struct inotify_event *event;
        int i;
        ssize_t len;
        char *ptr;

        /* Loop while events can be read from inotify file descriptor. */

        for (;;)
        {

            /* Read some events. */

            len = read(fd, buf, sizeof buf);
            if (len == -1 && errno != EAGAIN)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            /* If the nonblocking read() found no events to read, then
            it returns -1 with errno set to EAGAIN. In that case,
            we exit the loop. */

            if (len <= 0)
                break;

            /* Loop over all events in the buffer */

            for (ptr = buf; ptr < buf + len;
                 ptr += sizeof(struct inotify_event) + event->len)
            {

                event = (const struct inotify_event *)ptr;
                /* Print event type */

                if (event->mask & IN_OPEN)
                    printf("IN_OPEN: ");
                if (event->mask & IN_CLOSE_NOWRITE)
                    printf("IN_CLOSE_NOWRITE: ");
                if (event->mask & IN_CLOSE_WRITE)
                    printf("IN_CLOSE_WRITE: ");

                /* Print the name of the watched directory */

                for (i = 1; i < argc; ++i)
                {
                    if (wd[i] == event->wd)
                    {
                        printf("%s/", argv[i]);
                        break;
                    }
                }

                /* Print the name of the file */

                if (event->len)
                    printf("%s", event->name);

                /* Print type of filesystem object */

                if (event->mask & IN_ISDIR)
                    printf(" [directory]\n");
                else
                    printf(" [file]\n");
            }
        }
    }

    void test(int argc, char *argv[])
    {
        char buf;
        int fd, i, poll_num;
        int *wd;
        nfds_t nfds;
        struct pollfd fds[2];

        if (argc < 2)
        {
            printf("Usage: %s PATH [PATH ...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        printf("Press ENTER key to terminate.\n");

        /* Create the file descriptor for accessing the inotify API */

        fd = inotify_init1(IN_NONBLOCK);
        if (fd == -1)
        {
            perror("inotify_init1");
            exit(EXIT_FAILURE);
        }

        /* Allocate memory for watch descriptors */

        wd = calloc(argc, sizeof(int));
        if (wd == NULL)
        {
            perror("calloc");
            exit(EXIT_FAILURE);
        }

        /* Mark directories for events
        - file was opened
        - file was closed */

        for (i = 1; i < argc; i++)
        {
            wd[i] = inotify_add_watch(fd, argv[i],
                                      IN_OPEN | IN_CLOSE);
            if (wd[i] == -1)
            {
                fprintf(stderr, "Cannot watch '%s': %s\n",
                        argv[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        /* Prepare for polling */

        nfds = 2;

        /* Console input */

        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        /* Inotify input */

        fds[1].fd = fd;
        fds[1].events = POLLIN;

        /* Wait for events and/or terminal input */

        printf("Listening for events.\n");
        while (1)
        {
            poll_num = poll(fds, nfds, -1);
            if (poll_num == -1)
            {
                if (errno == EINTR)
                    continue;
                perror("poll");
                exit(EXIT_FAILURE);
            }

            if (poll_num > 0)
            {

                if (fds[0].revents & POLLIN)
                {

                    /* Console input is available. Empty stdin and quit */

                    while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                        continue;
                    break;
                }

                if (fds[1].revents & POLLIN)
                {

                    /* Inotify events are available */

                    handle_events(fd, wd, argc, argv);
                }
            }
        }

        printf("Listening for events stopped.\n");

        /* Close inotify file descriptor */

        close(fd);

        free(wd);
        exit(EXIT_SUCCESS);
    }

} // namespace ns_test1

namespace ns_test2
{ // from： https: //blog.csdn.net/nicai888/article/details/60954210

#define NAME_MAX 100
#define BUF_LEN (10 * sizeof(struct inotify_event) + NAME_MAX + 1)

    int get_inotify()
    {
        int fd;
        fd = inotify_init();
        if (fd == -1)
        {
            perror("inotify_init");
            return -1;
            // exit (EXIT_FAILURE);
        }

        std::string file_path = "/home/cn/work/cpp/LinuxC/linux-cpp/mysql/cmake";
        int wd = inotify_add_watch(fd, file_path.c_str(), IN_MODIFY | IN_ATTRIB);
        //wd = inotify_add_watch(fd, "/etc/", IN_ACCESS | IN_MODIFY);

        if (wd == -1)
        {
            perror("inotify_add_watch");
            return -1;
            // exit (EXIT_FAILURE);
        }
        char buf[BUF_LEN] __attribute__((aligned(4)));
        while (1)
        {
            ssize_t len, i = 0;
            /* read BUF_LEN bytes’ worth of events */
            len = read(fd, buf, BUF_LEN);
            /* loop over every read event until none remain */
            while (i < len)
            {
                struct inotify_event *event = (struct inotify_event *)&buf[i];
                printf("wd=%d mask=%d cookie=%d len=%d dir=%s\n",
                       event->wd, event->mask, event->cookie, event->len, (event->mask & IN_ISDIR) ? "yes" : "no");
                /* if there is a name, print it */
                if (event->len)
                {
                    printf("name=%s\n", event->name);
                }
                /* update the index to the start of the next event */
                i += sizeof(struct inotify_event) + event->len;
            }
        }
        return 0;
    }

    void main()
    {
        get_inotify();
    }
} // namespace ns_test2

namespace ns_test3
{
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

    void main()
    {
        std::cout << "test3:: main\n";
        char buffer[BUF_LEN];
        struct epoll_event ev, events[20];
        int epfd = epoll_create(256);
        int fd = inotify_init();

        int wd1 = inotify_add_watch(fd, "/home/cn/work/cpp/LinuxC/linux-cpp/mysql/cmake",
                                    IN_ALL_EVENTS);
        int wd2 = inotify_add_watch(fd, "/home/cn/docker/dockerfiles",
                                    IN_ALL_EVENTS);

        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLET;

        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

        for (;;)
        {
            int32_t nfds = epoll_wait(epfd, events, 20, 500);

            for (int i = 0; i < nfds; ++i)
            {
                if (events[i].data.fd == fd)
                {
                    int32_t length = read(fd, buffer, BUF_LEN);
                    if (length < 0)
                    {
                        perror("read");
                    }

                    while (i < length)
                    {
                        struct inotify_event *event =
                            (struct inotify_event *)&buffer[i];
                        if (event->len)
                        {
                            if (event->mask & IN_CREATE)
                            {
                                if (event->mask & IN_ISDIR)
                                {
                                    printf("The directory %s was created.\n",
                                           event->name);
                                }
                                else
                                {
                                    printf("The file %s was created.\n",
                                           event->name);
                                }
                            }
                            else if (event->mask & IN_DELETE)
                            {
                                if (event->mask & IN_ISDIR)
                                {
                                    printf("The directory %s was deleted.\n",
                                           event->name);
                                }
                                else
                                {
                                    printf("The file %s was deleted.\n",
                                           event->name);
                                }
                            }
                            else if (event->mask & IN_MODIFY)
                            {
                                if (event->mask & IN_ISDIR)
                                {
                                    printf("The directory %s was modified.\n",
                                           event->name);
                                }
                                else
                                {
                                    printf("The file %s was modified.\n",
                                           event->name);
                                }
                            }

                            std::cout << "event mask: " << event->mask << ", file: " << event->name << std::endl;
                        }
                        i += EVENT_SIZE + event->len;
                    }
                }
            }
        }
    }
} // namespace ns_test3

int main(int argc, char *argv[])
{
    daemon(1, 1);
    std::cout << "hello, world\n";
    //ns_test1::test(argc, argv);
    //ns_test2::main();
    ns_test3::main();
    return 0;
}