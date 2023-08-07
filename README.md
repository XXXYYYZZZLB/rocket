# rocket

TcpBuffer
应用层缓冲区
为什么需要应用层Buffer
- 方便数据处理，特别是应用层的包装和拆解
- 方便异步的发送（发送的数据直接塞到发送缓冲里面，等待epoll异步发送）
- 提高发送效率，多个包合并一起发送

关于TCP粘包：
tcp是没有粘包这个概念的，因为tcp本身是一个传输层的协议，tcp这一层是没有包这个概念的，
关于粘包说的是应用层，应用层是会有响应包，请求包等概念
例如HTTP，HTTP的一个请求体就是一个请求包，我们按照http协议的标准将字节流进行拆解，从中获取完整的HTTP请求包
在实际的网络环境中，tcp收到的可能不是一个完整的应用层包，就比如接受了一半，另外一半在socket缓冲区，我们是读还是不读呢
如果读了数据只用一半，那怎么办呢
所以，引入了一个应用层的buffer
读到一半的数据，我们会把它暂存到buffer里面，等待一个完整的数据，再进行解析
发送需要等待socket可写才能发送，如何实现异步发送：先暂存到buffer中，epoll监听socket可写时间，可写后发送缓冲区数据

tcpbuffer

        readIndex      writeIndex
            |              |
        [ ][b][c][f][g][t][ ][ ][ ][ ][ ][ ][ ]

        />回收调整<

        readIndex   writeIndex
        |              |
        [b][c][f][g][t][ ][ ][ ][ ][ ][ ][ ][ ] 平移过去

TcpAcceptor
    对socket创建到accept进行封装
    socket -> bind -> listen -> accept


TcpServer（主从Reactor）
主线程作用：
    通过epoll监听listenfd的可读事件，当可读事件发生后，调用acept函数获取 client_fd,
    随机取出一个subReactor，将client_fd的读写事件注册到这个subReactor的epoll上即可
    主线程只负责建立连接事件，不进行业务处理，也不关系已连接套接字的IO事件
subReactor:
    通常有多个，每个subReactor由一个线程来运行，其注册clientfd的读写事件，当发生IO后，需要进行业务处理






