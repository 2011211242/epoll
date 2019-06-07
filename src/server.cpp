#include "common.h"
#include <string>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/resource.h>

static int count = 0;

int main()
{
    int listenfd, connfd;
    struct sockaddr_in sockaddr;
    char buff[BUF_SIZE] = { 0 };

    int n;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(PORT);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if( fcntl( listenfd, F_SETFL, fcntl( listenfd, F_GETFD, 0 )|O_NONBLOCK ) == -1 )
    {
        printf("Set blocking error : %d\n", errno);
        return -1;
    }

    if ((bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) < 0)
    {
        fprintf(stderr, "%s bind error: %s, errno: %d\n", __FILE__, strerror(errno), errno);
        exit(0);
    }

    struct  epoll_event ev;
    struct  epoll_event evs[100000];
    int epoll_fd = epoll_create( 100000 );   //!> create  
    ev.events = EPOLLIN; //| EPOLLET;          //!> accept Read!  
    ev.data.fd = listenfd;                  //!> 将listen_fd 加入  

    if( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, listenfd, &ev ) < 0 )  
    {  
        printf("Epoll Error : %d\n", errno);  
        exit( EXIT_FAILURE );  
    }  
    int cur_fds = 1;  

    listen(listenfd, 1024);
    printf("Please wait for the client information\n");

    int accept_count = 0;
    for (;;)
    {
        int wait_fds = 0;
        if( ( wait_fds = epoll_wait( epoll_fd, evs, cur_fds, -1 ) ) == -1 )
        {
            printf( "Epoll Wait Error : %d\n", errno );
            exit( EXIT_FAILURE );
        }

        for( int i = 0; i < wait_fds; i++ )
        {
            if( evs[i].data.fd == listenfd && cur_fds < 100000)
            {
                int conn_fd = 0;
                if( ( conn_fd = accept( listenfd, (struct sockaddr *)NULL, NULL )) == -1 )
                {
                    printf("Accept Error : %d\n", errno);
                    exit( EXIT_FAILURE );
                    accept_count ++;
                    printf("accept: %d\n", accept_count);
                }

                struct timeval timeout={3, 50};  //3s
                int ret=setsockopt(conn_fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
                if (ret != 0)
                {
                    printf("set time out error\n");
                }

                ev.events = EPOLLIN; // | EPOLLET;      //!> accept Read!
                ev.data.fd = conn_fd;               //!> 将conn_fd 加入
                if( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev ) < 0 )
                {
                    printf("Epoll Error : %d\n", errno);
                    exit( EXIT_FAILURE );
                }
                ++cur_fds;
                continue;
            }
            printf("accept_count = %d\n", accept_count);

            int nread = read( evs[i].data.fd, buff, sizeof( buff ) );
            if( nread <= BUF_SIZE)
            {
                close( evs[i].data.fd );
                epoll_ctl( epoll_fd, EPOLL_CTL_DEL, evs[i].data.fd, &ev );  //!> 删除计入的fd
                --cur_fds;                  //!> 减少一个呗！
                printf("read fail, cur_fds: %d, nread: %d, %s\n", cur_fds, nread, buff);
                continue;
            }

            //printf("read succ, cur_fds: %d, nread: %d, %s\n", cur_fds, nread, buff);
            //epoll_ctl( epoll_fd, EPOLL_CTL_MOD, evs[i].data.fd, &ev );  //!> 删除计入的fd
            //printf("nread: %d\n", nread);
        }
    }
    close(epoll_fd);
}
