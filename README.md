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

# 2.22 第一版聊天室开发完成
第一版可以实现简单的服务器与客户端通信，具体效果如下：
server:
![Image test]( https://github.com/SalvationN/SimpleChatRoom/tree/master/images/server v1.png )
client:
![Image test]( https://github.com/SalvationN/SimpleChatRoom/tree/master/images/client v1.png )
## 排坑调试
由于一些细微的括号处理问题，导致出现Segmentation fault (core dumped)错误，core dumped是核心转储。进程异常终止，进程用户空间的数据就会被写到磁盘。但是linux下一般不会产生core文件，因此需要用到gbt调试。  
- ulimit -c 1024修改corefilesize，然后再次运行程序
- 这时出现core.xxx文件
- 然后用gbt ./client core.xxx即可得到错误代码的原因和出错地方

# TCP协议选项
在TCP头部，为了更好地服务应用层而设计。主要有以下几种：
1. SO_REUSEADDR  
在2.22的实验中，在强制关闭server.cpp代码，然后再次运行时，会出现地址端口被占用的情况。一般来说，端口释放后需要两分钟左右后才能再次被使用。SO_REUSEADDR能够针对TIMEWAIT(先挥手的一方)在状态下的socket进行重复绑定使用。SO_REUSEADDR提供如下四种功能：
- 允许启动一个监听服务器并绑定一个公认的端口，以前将用该端口建立的连接仍然存在。
- 允许在同一端口上启动**同一**服务器的多个**实例**，只要每个实例都绑定不同的本地ip
- 允许单进程绑定同一个端口到多个socket上
- 允许完全重复的捆绑，即同一ip和端口号可以绑定到不同的socket上；但此特性只适合多播(UDP)
2. TCP_NODELAY/TCP_CHORK
- Nagle算法：针对连续多个小数据包的发送导致的额外开销问题(数据包头很大，有效数据很小)，核心思想是在一段时间内只能有一个小数据包，其他小数据包会被缓存并组成一个大数据包，直到收到前一个数据包的ACK。
- TCP中Nagle算法默认启用，但有些场景需要关闭它
- TCP_NODELAY和TCP_CHORK就实现了Nagle算法的关闭
- TCP_NODELAY直接将小包发出去，而TCP_CHORK则会每次保证200ms的阻塞时间，阻塞时间过后发送数据
3. SO_LINGER —— 延迟关闭   
linger可以改变面向连接的socket的close操作——即向对方挥手，它的数据结构中包含两个参数：l_onoff和,l_linger
- l_onoff=0，l_linger忽略，close()直接返回
![Image test]( https://github.com/SalvationN/SimpleChatRoom/tree/master/images/close-return-1.png )
- l_onoff非0，l_linger=0，则close()立即执行，且丢弃缓冲区的数据(即强制关闭)，并向对端发送一个RST报文
- l_onoff和l_linger均非0，则阻塞close()，若缓冲区内还有数据，则当数据发送完毕或延迟时间消耗完，close()阻塞结束
![Image test]( https://github.com/SalvationN/SimpleChatRoom/tree/master/images/close-return-2.png )
实际上这种情况是保证了对端已经安全接收了所有的数据，而默认情况是发完数据就完了，不理会对方是否成功接收
4. TCP_DEFER_ACCEPT —— 延迟接收  
使用方法：sersockopt(srv_socket->fd, SOL_TCP, TCP_DEFER_ACCEPT, &val, sizeof(val))；  
打开该功能后，经过三次握手，新的socket状态依然是SYN_RECV，而不会进入ESTABLISHED，如果经过val时间，仍然没有数据发送，服务器会重传SYN_ACK报文，若重传达到重传次数，该连接将被丢弃
5. SO_KEEPALIVE —— 检测对方主机是否崩溃  
6. SO_SNDTIMEO/SO_RECVTIMEO —— 设置socket发送和接收超时时间
