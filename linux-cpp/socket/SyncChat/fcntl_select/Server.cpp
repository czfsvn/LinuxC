//from: http://outofmemory.cn/code-snippet/7749/linux-socket-network-program-fcntl-select-duoge-customer-duan-connection-service-qi-duan-qingxing
//


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define     SERVER_PORT     3333
#define     BACKLOG         10

