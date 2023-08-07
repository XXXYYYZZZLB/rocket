#ifndef ROCK_NET_TCP_TCP_SERVER
#define ROCK_NET_TCP_TCP_SERVER

#include "rocket/net/tcp/tcp_acceptor.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/io_thread_group.h"

namespace rocket
{
    class TcpServer
    {
    public:
        TcpServer(NetAddr::s_ptr local_addr);
        ~TcpServer();

        void start();
        void init();
        // 当有新连接
        void onAccept();

    private:
        TcpAcceptor::s_ptr m_acceptor;
        NetAddr::s_ptr m_local_addr; // 本地监听地址

        EventLoop *m_main_event_loop; // mainReactor

        IOThreadGroup *m_io_thread_groups; // subReactor组

        FdEvent *listen_fd_event{NULL};

        int m_client_count{0};
    };

}

#endif