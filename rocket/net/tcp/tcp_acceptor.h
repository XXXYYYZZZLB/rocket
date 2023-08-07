#ifndef ROCKET_NET_TCP_TCP_ACCEPTOR_H
#define ROCKET_NET_TCP_TCP_ACCEPTOR_H

#include "rocket/net/tcp/net_addr.h"
#include <memory>

namespace rocket
{
    class TcpAcceptor
    {
    public:
        typedef std::shared_ptr<TcpAcceptor> s_ptr;
        TcpAcceptor(NetAddr::s_ptr local_addr);
        ~TcpAcceptor();
        int accept();

        int getListenFd();

    private:
        NetAddr::s_ptr m_local_addr; // 服务端监听地址 addr--ip:port
        int m_listenfd{0};           // 监听套接字
        int m_family{-1};            // 地址协议族
    };

} // namespace rocket

#endif