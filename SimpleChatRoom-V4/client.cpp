//头文件
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<poll.h>

#define MAXLINE 1024
#define DEFAULT_PORT 6666
#define max(a,b) (a>b)?a:b


//封装好的连接代码
static void handle_connection(int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    int maxfd,stdineof;
    struct pollfd pfds[2];
    int n;
    //pfds[0]是连接描述符
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;
    //pfds[1]是标准输入描述符
    pfds[1].events = POLLIN;
    pfds[1].fd = STDIN_FILENO;
    while(1)
    {
        poll(pfds,2,-1);
        if(pfds[0].revents & POLLIN)
        {
            n = read(sockfd,recvline,MAXLINE);
            if(n==0)
            {
                printf("服务器已关闭！\n");
                close(sockfd);
            }
            write(STDOUT_FILENO,recvline,n);
        }
        if(pfds[1].revents & POLLIN)
        {
            printf("请输入：\n");
            n = read(STDIN_FILENO,sendline,MAXLINE);
            if(n==0)
            {
                close(sockfd);
                continue;
            }
            write(sockfd,sendline,n);
            printf("%s\n",sendline);
        }
    }
}

int main(int argc,char** argv){
    int sockfd,n;
    struct sockaddr_in servaddr;
    //参数错误提示
    if( argc != 2 ){
        printf("usage：.client <ipaddress>\n");
        return 0;
    }

    //创建客户端socket，无需bind过程，有os自动分配
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        printf("创建socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return 0;
    }
    

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM,0);


    //连接服务器
    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) ) < 0){
        printf("连接错误：%s(errno:%d)\n",strerror(errno),errno);
        return 0;
    }   
    handle_connection(sockfd);
    return 0;
}
