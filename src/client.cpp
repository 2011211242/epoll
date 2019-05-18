#include "common.h"
#include <mutex>
#include <thread>
#include <vector>

static int thread_num = 7;
static std::mutex mtx;
static int count = 0;
#define THREAD_NUM 7
#define CONNECT_NUM 100000
void client_func() 
{
    mtx.try_lock();
    int cnt = count;
    mtx.unlock();

    while(cnt <= CONNECT_NUM - THREAD_NUM)
    {
        int socketfd = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(PORT);
        inet_pton(AF_INET, IP, &sockaddr.sin_addr);
        if ((connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0)
        {
            fprintf(stderr, "%s connect server error %s errno: %d\n", __FILE__, strerror(errno), errno);
            //exit(0);
            close(socketfd);
            continue;
        }

        char msg[] = "hello server";
        if((send(socketfd, msg, strlen(msg), 0)) < 0)
        {
            fprintf(stderr, "%s send msg to server error %s errno: %d\n", __FILE__, strerror(errno), errno);
        }

        char buff[BUF_SIZE];
        int n = recv(socketfd, buff, BUF_SIZE, 0);

        if (n <= 0)
        {
            buff[n] = '\0';
            printf("%s recv msg from server: %s\n", __FILE__, buff);
        }

        close(socketfd);
        //printf("%s recv msg from server: %s, count: %d\n", __FILE__, buff, cnt);
        while( !mtx.try_lock() )
        {
            sleep(1);
            count ++;
            cnt = count;
            mtx.unlock();
        }
    }
    printf("exit\n");
}

int main()
{
    std::vector<std::thread> thread_vec(THREAD_NUM);

    for (int i = 0; i < thread_vec.size(); i++)
    {
        thread_vec[i] = std::thread(client_func);
    }

    for (int i = 0; i < thread_vec.size(); i++)
    {
        thread_vec[i].join();
    }

    return 0;

}
