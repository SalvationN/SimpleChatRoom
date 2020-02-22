//头文件
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define MAXLINE 4096

int main(int argc,char** argv){
    int listenfd,connfd;
    struct sockaddr_in servaddr;	//sockaddr_in格式的服务器地址
    char buff[4096];	//允许接受的最大缓冲区
    int n;
    
    //创建了AF_INET协议域，TCP类型的socket，若创建失败(返回-1)，则打印出错误信息及其编号
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1 ){
    	printf("创建socket失败： %s(errno: %d)\n",strerror(errno),errno);
	    return 0;
    }
    
    
    /* 
     初始化服务器地址，协议族为AF_INNET，地址为INADDR_ANY，即0.0.0.0，表示不确定地址，该地址用于多网卡机器，这样能保证其他机器通过不同的ip访问该机器;
     htonl函数将主机字节顺序数转换成无符号长整形的网络字节顺序，因为ipv4地址结构要求是网路字节序；与该函数相对应的是ntohl()；
     下附ipv4 socket地址结构：
     struct in_addr{
         in_addr_t    s_addr;          //32位ipv4地址
     };

     struct sockaddr_in{
        uint8_t      sin_len;      // length of structure (16)
        sa_family_t  sin_family;   // AF_INET
        in_port_t    sin_port;     // 16-bit TCP or UDP port number, network byte ordered
        struct in_addr sin_addr;   // 32-bit IPv4 address, network byte ordered
     };
     */ 
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);
    //以上是监听socket创建过程


    //绑定socket地址
    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1 ){
        printf("bind socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return 0;
    }


    //监听这个地址
    if(listen(listenfd, 10) == -1){
        printf("监听socket错误：%s(errno:%d)\n",strerror(errno),errno);
    }


    //在接收客户端信息前，阻塞式accept会一直等待，接受到信息后，第二个参数所指向的socket会被填入客户端的信息
    printf("========等待客户端请求===========\n");
    while(1){
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket错误：%s(errno:%d)\n",strerror(errno),errno);
        }
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';      //recv函数接受的不包括结束符，因此要加上
        printf("已接收来自客户端的信息：%s\n", buff);
    }
    
    close(listenfd);
    return 0;
}
