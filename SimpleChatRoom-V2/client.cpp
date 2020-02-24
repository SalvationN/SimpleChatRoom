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
    char recvline[4096],sendline[4096];
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


    /*
     * ======第二版新增——超时时间设置========
     */
   
    struct timeval stTimeValStructor;
    stTimeValStructor.tv_sec = 2;       //超时时间设为两秒
    stTimeValStructor.tv_usec = 0;      //这个是毫秒级
    //发送和接受时间都设置为2秒
    if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &stTimeValStructor, sizeof(stTimeValStructor)) < 0){
        printf("协议选项配置错误：%s(errno:%d)\n", strerror(errno), errno);
        return 0;
    }
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &stTimeValStructor, sizeof(stTimeValStructor)) < 0){
        printf("协议选项配置错误：%s(errno:%d)\n", strerror(errno), errno);
        return 0;
    }

    //往服务端发送一段字符串
    ssize_t writenLen;      //注:ssize_t大致相当于long int
    char sendMsg[6] = {'a','b','c','d','e','\0'};
    int cnt = 0;
    ssize_t writeLen = write(sockfd, sendMsg, sizeof(sendMsg));
    if(writeLen<0){
        printf("写入错误：%s(errno:%d)\n", strerror(errno), errno);
        close(sockfd);
        return 0;
    }else{
        printf("已成功写入，长度为:%d，内容为:%s\n", writeLen, sendMsg);
    }

    //接收服务器回包
    char readMsg[6] = {0};
    ssize_t readLen = read(sockfd, readMsg, sizeof(readMsg));
    if(readLen<0){
        printf("读入错误：%s(errno:%d)\n", strerror(errno), errno);
        close(sockfd);
        return 0;
    }else{
        readMsg[5] = '\0';
        printf("已成功读入，长度为:%d，内容为:%s\n", readLen, readMsg);
    }

    close(sockfd);
    return 0;
}
