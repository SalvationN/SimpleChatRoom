//V4————poll版
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<poll.h>

#define MAXLINE 1024
#define IPADDRESS "127.0.0.1"
#define PORT 6666
#define LISTENQ 5
#define OPEN_MAX 1000
#define INFTIM  -1


//监听和绑定
int bind_and_listen(){
    int listenfd;     //监听的fd和accept的fd
    struct sockaddr_in servaddr;	//sockaddr_in格式的服务器地址
    unsigned int sin_size;
    
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1 ){
    	printf("创建socket失败： %s(errno: %d)\n",strerror(errno),errno);
	    return 0;
    }
    printf("socket创建成功！\n");
    
     
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    bzero(&(servaddr.sin_zero),0);
    //以上是监听socket创建过程


    //绑定socket地址
    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1 ){
        printf("bind socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return -1;
    }
    printf("地址绑定成功！\n");

    //监听这个地址
    if(listen(listenfd, LISTENQ) == -1){
        printf("监听socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return -1;
    }
    printf("监听成功！\n");
    return listenfd;
}


//第四版核心功能，poll
void do_poll(int listenfd)
{
    int connfd,sockfd;
    struct sockaddr_in cltaddr;
    socklen_t cltaddrlen;
    struct pollfd clientfds[OPEN_MAX];
    int maxi,i;
    int nready;
    
    clientfds[0].fd = listenfd;     //监听面束缚必须放入描述符集里
    clientfds[0].events = POLLIN;
    
    for(i=1;i<OPEN_MAX;i++)
        clientfds[i].fd=-1;
    maxi=0;
    while(1)
    {
        nready = poll(clientfds,maxi+1,INFTIM);
        //printf("有%d个客户端准备好了！\n",nready);
        if(nready == -1)
        {
            perror("poll错误！\n");
            exit(1);
        }
        if(clientfds[0].revents & POLLIN)
        {
            cltaddrlen = sizeof(cltaddr);
            if((connfd=accept(listenfd, (struct sockaddr*)(&cltaddr), &cltaddrlen))==-1)
            {
                if(errno == EINTR)continue;
                else
                {
                    perror("accept 错误！\n");
                    exit(1);
                }
            }
            printf("接收了一个新客户端，地址为：%s，端口为：%d。\n",inet_ntoa(cltaddr.sin_addr),cltaddr.sin_port);
            for(i=1;i<OPEN_MAX;i++)
            {
                if(clientfds[i].fd<0)
                {
                    clientfds[i].fd=connfd;         //找出最小的空fd，即将connfd加入到集合中
                    //printf("新加入的fd:%d\n",i);
                    break;
                }
            }
            if(i==OPEN_MAX)
            {
                fprintf(stderr,"客户端太多拉！\n");
                exit(1);
            }
            clientfds[i].events = POLLIN;
            maxi = (i>maxi?i:maxi);
            if(--nready<=0)continue;
        }
        char buff[MAXLINE];
        memset(buff,0,MAXLINE);
        int readlen=0;
        for(i=1;i<=maxi;i++)
        {
            //printf("调试第%d步:\n",i);
            if(clientfds[i].fd<0)continue;
            if(clientfds[i].revents & POLLIN)
            {
                readlen = read(clientfds[i].fd,buff,MAXLINE);
                if(readlen==0){
                close(clientfds[i].fd);
                clientfds[i].fd=-1;
                continue;
                }
                printf("读取数据：%s\n",buff);
                write(STDOUT_FILENO,buff,readlen);
                write(clientfds[i].fd,buff,readlen);
            }
        }
    }
}
int main(int argc,char* argv[]){
    int listenfd = bind_and_listen();
    if(listenfd<0)return 0;
    do_poll(listenfd);
    return 0;
}
