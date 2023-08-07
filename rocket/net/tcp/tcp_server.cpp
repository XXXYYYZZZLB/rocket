#include "rocket/net/tcp/tcp_server.h"
#include "rocket/net/eventloop.h"
#include <stdio.h>

namespace rocket
{

    TcpServer::TcpServer(NetAddr::s_ptr local_addr) : m_local_addr(local_addr)
    {
        init();

        INFOLOG("rocket RPC server succ on [%s]", m_local_addr->toString().c_str());
    }
    TcpServer::~TcpServer()
    {
        if (m_main_event_loop)
        {
            delete m_main_event_loop;
            m_main_event_loop = NULL;
        }
        if (m_io_thread_groups)
        {
            delete m_io_thread_groups;
            m_io_thread_groups = NULL;
        }
    }

    void TcpServer::onAccept()
    {
        int client_fd = m_acceptor->accept();
        m_client_count++;
        // FdEvent client_fd_event(client_fd);
        //  m_io_thread_groups->getIOThread()->getEventLoop()->addEpollEvent(&client_fd_event);

        // TODO: 把client_fd 添加到任意IO线程中
        printf("Tcpserver succ get client!");
    }

    void TcpServer::init()
    {

        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);
        m_main_event_loop = EventLoop::GetCurrentEventLoop();
        m_io_thread_groups = new IOThreadGroup(2);

        // 局部变量不可以传指针，有线程安全问题，执行完会析构，发生段错误
        listen_fd_event = new FdEvent(m_acceptor->getListenFd());
        listen_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
        m_main_event_loop->addEpollEvent(listen_fd_event);
    }

    void TcpServer::start()
    {
        m_io_thread_groups->start();
        m_main_event_loop->loop();
    }

} // namespace rocket
