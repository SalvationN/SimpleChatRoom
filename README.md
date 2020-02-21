# 前言
本项目的目标是用linux C++实现一个简易的聊天室，目的是为了通过该项目了解socket网络编程，并进一步理解计算机网络相关原理。简易聊天室这个项目参考资料较多，争取一周之内完成该项目，并且深刻理解其中的原理。
# socket概述
socket是Unix/linux下的一种特殊文件，用于网络中的进程通信，socket函数就是对该文件进行的一系列读写开关操作。下面是TCP协议下的交互流程图：
！[Image test](https://github.com/SalvationN/SimpleChatRoom/images/socket交互.png)
## TCP协议下的交互流程
- 服务器根据地址类型、socket类型、协议创建socket，并绑定ip地址和端口号
- 服务器socket监听端口号请求，随时准备接收客户端发来的连接。（此时的socket并没有被打开）
- 客户端创建socket，并根据端口和ip试图连接服务器
- 服务器socket接收到请求，被动打开，开始接收客户端请求，直到客户端返回连接信息。这时socket进入阻塞状态。
- 客户端连接成功，向服务器发送连接信息。（三次握手...）
- 服务器accept()方法返回，连接成功。
- 客户端向socket写入信息
- 服务器读取信息
- 客户端关闭
- 服务器端关闭（四次挥手...）  

## socket函数介绍
1. socket()
原型：int socket(int domain,int type,int protocol);  
- socket()类似文件打开操作，返回一个socket()描述符，用于标识socket。
- domain：协议域，如AF_INET、AF_INET6、AF_LOCAL(AF_UNIX)等，协议决定了地址类型。
- type：指定socket类型，如SOCK_STREAM、SOCK_DGRAM、SOCK_RAW、SOCK_PACKET等。SOCK_STREAM是面向连接、稳定的，SOCK_DGRAM是不连续不稳定的
- protocol：指定协议，分别为IPPOTO_TCP、IPPOTO_UDP、IPPOTO_SCTP、IPPOTO_TIPC，后缀分别对应其指定的协议  
注：type和protpcol不能随意组合，SOCK_STREAM必须是TCP协议  
当调用一个socket时，返回的socket描述字存在于协议簇中，没有具体地址。要想有地址，必须调用bind函数，如果没有调用bind，则在connect和listen时自动分配端口。  
每个进程空间中都会有套接字描述表，该表存放套接字描述符和套接字数据结构(在操作系统内核)的关系。
2. bind()
原型：int bind(int sockfd,const struct sockaddr* addr, socklen_t* addrlen);  
- sockfd：即socket()返回的socket描述符
- addr：指向要绑定的socket地址，地址结构根据创建socket时的domain来决定
- addrlen：地址长度  
由于服务器端通常会绑定一个地址以便各客户端连接，因此服务器端需要在创建之后使用bind()；而客户端只需要connect时由服务器分配一个端口即可。  
3. listen()和connect()
原型： int listen(int sockfd,int backlog);int connect(int sockfd,const struct sockaddr* addr, socklen_t* addrlen);  
- listen函数中的backlog是指socket可以排队的最大连接个数，即如果有多个客户端请求连接，则最多只能有backlog个
4. accept()
原型： int accept(int sockfd,const struct sockaddr* addr, socklen_t* addrlen);  
- accept函数是在服务器接收到连接请求后，调用accept()函数继续接收请求。
- 第一个参数为服务器最开始调用的socket的描述字，称为socket监听字；而accept返回值是已连接的socket描述字。（一个服务器只创建一个socket监听字，而为每个请求连接并被接收的客户端创建已连接的socket描述字，当服务关闭时，该描述字就被关闭了）
- 第二个参数用于返回客户端的协议地址
5. read()和write()
- read: ssize_t read(int fd,void* buf,size_t count);
- 三个参数分别是：socket描述符，缓冲区，缓冲区长度；该函数即从fd中读取内容。read()返回实际所读的字节数，如果返回值是0表示已经读到文件的结束了。
- write：ssize_t read(int fd,void* buf,size_t count);
- write返回所写的字节数
6. close()
- 需要包含头文件< unistd >
- socket调用close后，会把该socket标记为关闭，然后立即返回到调用进程。

