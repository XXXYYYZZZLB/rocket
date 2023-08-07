#include "rocket/common/log.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_buffer.h"
#include "rocket/net/tcp/tcp_acceptor.h"
#include <assert.h>
#include <sys/socket.h>
#include <string.h>

namespace rocket
{

    TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr) : m_local_addr(local_addr)
    {
        if (!local_addr->checkValid())
        {
            ERRORLOG("检查到地址不合法");
            exit(0);
        }

        m_family = m_local_addr->getFamily();
        m_listenfd = socket(m_family, SOCK_STREAM, 0);
        if (m_listenfd < 0)
        {
            ERRORLOG("m_listenfd = socket(m_family,SOCK_STREAM,0) 错误");
            exit(0);
        }

        // 设置端口复用，服务器快速重启
        int val = 1;
        if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0)
        {
            ERRORLOG("setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) 错误");
        }

        socklen_t len = m_local_addr->getSockLen();
        if (bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0)
        {
            ERRORLOG("bind 错误");
        }

        if (listen(m_listenfd, 1000) != 0)
        {
            ERRORLOG("listen 错误");
            exit(0);
        }
    }

    int TcpAcceptor::accept()
    {
        if (m_family == AF_INET)
        {
            sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t client_addr_len = sizeof(client_addr);

            int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
            if (client_fd < 0)
            {
                ERRORLOG("::accept 错误");
            }
            IPNetAddr peer_addr(client_addr);
            INFOLOG("A client have accepted succ! [%s]", peer_addr.toString());
            return client_fd;
        }
        else
        {
            // TODO 实现其他的方法
        }
    }

    TcpAcceptor::~TcpAcceptor()
    {
    }

}