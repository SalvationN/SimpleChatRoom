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

/*
    这里说明以下带参数的main函数各参数的意义： 
    第一个参数，int型的argc，为整型，用来统计程序运行时发送给main函数的命令行参数的个数；
    第二个参数，char*型的argv[]，为字符串数组，用来存放指向的字符串参数的指针数组，每一个元素指向一个参数。各成员含义如下： 
        argv[0]指向程序运行的全路径名； 
        argv[1]指向在DOS命令行中执行程序名后的第一个字符串； 
        argv[2]指向执行程序名后的第二个字符串； 
        argv[3]指向执行程序名后的第三个字符串；  
*/
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

    //发送
    printf("send msg to server:\n");
    fgets(sendline, 4096, stdin);
    if( send(sockfd, sendline, strlen(sendline), 0) < 0 ){
        printf("连接错误：%s(errno:%d)\n",strerror(errno),errno);
        return 0;
    }

    close(sockfd);
    return 0;
}
