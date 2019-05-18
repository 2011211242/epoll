#include "common.h"

static int count = 0;

int main()
{
    int listenfd, connfd;
    struct sockaddr_in sockaddr;
    char buff[BUF_SIZE] = { 0 };
    char msg[] = "hello client";
    int n;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(PORT);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if ((bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) < 0)
    {
        fprintf(stderr, "%s bind error: %s, errno: %d\n", __FILE__, strerror(errno), errno);
        exit(0);
    }

    listen(listenfd, 1024);

    printf("Please wait for the client information\n");
    for (;;)
    {
        if((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) < 0)
        {
            fprintf(stderr, "%s accept socket error: %s, errno: %d\n", __FILE__, strerror(errno), errno);
            continue;
        }


        n = recv(connfd, buff, BUF_SIZE, 0);
        buff[n] = '\0';
        count++;
        if (n <= 0)
        {
            printf("%s: recv msg from client:%s count:%d\n", __FILE__, buff, count);
        }
        else {
            printf("count: %d\n", count);
        }

        int send_ret = send(connfd, msg, strlen(msg), 0);
        if (send_ret < 0)
        {
            fprintf(stderr, "%s:%d send_ret: %d, send msg error: %s, errno: %d\n", __FILE__, __LINE__, send_ret, strerror(errno), errno);
        }
        close(connfd);
    }

    close(listenfd);
    return 0;
}
