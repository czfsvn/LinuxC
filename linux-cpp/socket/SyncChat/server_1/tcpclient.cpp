//from : http://outofmemory.cn/code-snippet/1015/Linux-Socket-program-example-single-Hello-World-program


#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define  SERVER_PORT 3838                       //  define the defualt connect port id
#define  CLIENT_PORT ((20001+rand())%65536)     //  define the defualt client port as a random port
#define  BUFFER_SIZE 255
#define  REUQEST_MESSAGE "welcome to connect the server.\n"

int main()
{
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0)
    {
        printf("create socket error!\n");
        exit(1);
    }
    printf("create socket success!\n");

    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("connect socket error!\n");
        exit(1);
    }
    printf("connect socket success !\n");

    char buf[BUFFER_SIZE] = {0};
    int length = recv(clientfd, buf, BUFFER_SIZE, 0);
    if (length < 0)
    {
        printf("recv socket error!\n");
        exit(1);
    }

    printf("from server %s :\n\t", buf);
    close(clientfd);
    return 0;
}
