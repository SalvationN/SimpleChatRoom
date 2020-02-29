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
    //inet_pton将点分十进制数转换成无符号的网络字节序数，其中第二个参数为点分十进制数指针
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0 ){
        printf("inet_pton地址转换错误：%s\n",argv[1]);
        return 0;
    }


    //连接服务器
    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) ) < 0){
        printf("连接错误：%s(errno:%d)\n",strerror(errno),errno);
        return 0;
    }
    char buff[4096];
    bzero(buff,sizeof(buff));
    recv(sockfd, buff, 4096, 0);
    printf("收到来自服务器的消息：%s\n",buff);
    bzero(buff,sizeof(buff));
    strcpy(buff,"这是客户端的信息呦！\n");
    send(sockfd, buff, 1024, 0);
    while(1)
    {
        bzero(buff,sizeof(buff));
        scanf("%s",buff);
        int p = strlen(buff);
        buff[p] = '\0';
        send(sockfd,buff,4096,0);
        printf("已成功发送！\n");
    }
    
    close(sockfd);
    return 0;
}
