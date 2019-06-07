#include "common.h"
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>

static int thread_num = 7;
static std::mutex mtx;
static int count = 0;
#define THREAD_NUM 1200
#define CONNECT_NUM 100000

bool sendall() {
    mtx.lock();
    count ++;
    int cnt = count;
    printf("s_count: %d\n", cnt);
    mtx.unlock();
    return cnt > CONNECT_NUM;
}

void client_func() 
{
    //int s_count = 0;
    while(sendall() == false)
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

        sleep(3);
        char msg[] = "hello server";
        if((send(socketfd, msg, strlen(msg), 0)) <= 0)
        {
            fprintf(stderr, "%s send msg to server error %s errno: %d\n", __FILE__, strerror(errno), errno);
        }
        //s_count ++;
        close(socketfd);
    }
    printf("exit\n");
}

int main()
{
    std::vector<std::thread> thread_vec(THREAD_NUM);

    for (int i = 0; i < thread_vec.size(); i++)
    {
        thread_vec[i] = std::thread(client_func);
        usleep(5000);
    }

    for (int i = 0; i < thread_vec.size(); i++)
    {
        thread_vec[i].join();
    }

    return 0;

}
