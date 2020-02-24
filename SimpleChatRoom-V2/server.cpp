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
    
    //创建了AF_INET协议域，TCP类型的socket，若创建失败(返回-1)，则打印出错误信息及其编号
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1 ){
    	printf("创建socket失败： %s(errno: %d)\n",strerror(errno),errno);
	    return -1;
    }
    
     
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


    //监听这个地址
    if(listen(listenfd, 10) == -1){
        printf("监听socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return -1;
    }


    //超时设置
    printf("========等待客户端请求===========\n");
    if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
        printf("accept socket错误：%s(errno:%d)\n",strerror(errno),errno);
        return 0;
    }
    char recvMsg[100];
    ssize_t readLen = read(connfd, recvMsg, sizeof(recvMsg));
    if(readLen < 0){
        printf("读入错误：%s(errno:%d)\n",strerror(errno),errno);
        return -1;
    }
    recvMsg[5] = '\0';
    printf("接收长度：%d，接收内容:%s\n",readLen,recvMsg);
    //睡眠5秒后修改接收值再发送回去
    sleep(5);
    recvMsg[1] = 'z';
    ssize_t writeLen = write(connfd, recvMsg, sizeof(recvMsg));
    if(writeLen<0){
        printf("写入错误：%s(errno:%d)\n", strerror(errno), errno);
        return 0;
    }
   
    close(connfd);
    return 0;
}
