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

#define MAXLINE 4096

int main(int argc,char** argv){
    int listenfd,acceptfd;     //监听的fd和accept的fd
    struct sockaddr_in servaddr;	//sockaddr_in格式的服务器地址
    struct sockaddr_in cltaddr;     //客户端地址
    char buff[4096];	//允许接受的最大缓冲区
    unsigned int sin_size,myport=6666,lisnum=10;
    int n;
    
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1 ){
    	printf("创建socket失败： %s(errno: %d)\n",strerror(errno),errno);
	    return 0;
    }
    printf("socket创建成功！\n");
    
     
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);
    //以上是监听socket创建过程


    //绑定socket地址
    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1 ){
        printf("bind socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return -1;
    }
    printf("地址绑定成功！\n");

    //监听这个地址
    if(listen(listenfd, 10) == -1){
        printf("监听socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return -1;
    }
    printf("监听成功！\n");



    //第三版核心功能，配置select参数
    fd_set client_fdset;        //监控的文件描述符集合
    int maxsock;        //架空文件描述符的最大文件号
    struct timeval tv;      //超时返回时间
    int client_sockfd[5];       //存放活动的fd
    bzero((void*)client_sockfd,sizeof(client_sockfd));
    int conn_amount = 0;        //记录描述符数量
    maxsock = listenfd;
    int ret=0;

    //在接收客户端信息前，阻塞式accept会一直等待，接受到信息后，第二个参数所指向的socket会被填入客户端的信息
    printf("========等待客户端请求===========\n");
    while(1){
        FD_ZERO(&client_fdset);     //将fd_set清零
        FD_SET(listenfd,&client_fdset);     //将listenfd加入fdset
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        //将所有已经连上的client加入到集合
        for(int i=0;i<5;++i)
        {
            if(client_sockfd[i]!=0)
                FD_SET(client_sockfd[i],&client_fdset);
        }

        ret = select(maxsock+1, &client_fdset, NULL, NULL, &tv);        //这个版本只监听是否有数据读，所以第三四个参数为NULL
        if(ret<=0)
        {
            perror("select 错误！\n");break;            
        }
        else if(ret==0)
        {
            printf("超时！\n");
            continue;
        }
        
        //有新消息了，需逐个扫描
        for(int i=0;i<conn_amount;i++)
        {
            if(FD_ISSET(client_sockfd[i], &client_fdset))
            {
                printf("开始从第%d个客户端接收数据......\n",i);
                ret = recv(client_sockfd[i],buff,4096,0);
                //对方socket已关闭的处理
                if(ret<=0)
                {
                    printf("客户端%d已关闭!\n",i);
                    close(client_sockfd[i]);
                    FD_CLR(client_sockfd[i],&client_fdset);
                    client_sockfd[i]=0;
                }
                else printf("成功接收数据：%s\n",buff);
            }
        }
        
        if(FD_ISSET(listenfd, &client_fdset))
        {
            struct sockaddr_in client_addr;
            size_t size = sizeof(struct sockaddr_in);
            int socket_client = accept(listenfd, (struct sockaddr*)(&client_addr), (unsigned int *)(&size));
            if(socket_client<0)
            {
                perror("accept错误！\n");
                continue;
            }

        if(conn_amount<5)
        {
            client_sockfd[conn_amount++] = socket_client;
            bzero(buff,4096);
            strcpy(buff,"this is server! Welcom!\n");
            send(socket_client, buff, 4096, 0);
            printf("与客户端[%d]的连接建立了！地址为:%s，端口为：%d\n", conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            bzero(buff, sizeof(buff));
            ret = recv(socket_client,buff,4096,0);
            if(ret<=0)
            {
                printf("客户端已关闭!\n");
                close(socket_client);
                return -1;
            }
            else printf("成功接收数据：%s\n",buff);
            if(socket_client>maxsock){
                maxsock = socket_client;
            }
            else
            {
                printf("连接已满！！退出！！\n");
                break;
            }
        }
    }
    }
    for(int i=0;i<5;++i){
        if(client_sockfd[i]!=0)close(client_sockfd[i]);
    }
    close(listenfd);
    return 0;
}
