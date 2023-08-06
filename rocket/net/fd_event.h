#ifndef ROCKET_NET_FD_EVENT_H
#define ROCKET_NET_FD_EVENT_H

#include <functional>
#include <sys/epoll.h>

namespace rocket
{
    class FdEvent
    {
    public:
        enum TriggerEvent
        {
            IN_EVENT = EPOLLIN,
            OUT_EVENT = EPOLLOUT,
        };

        FdEvent(int fd);
        ~FdEvent();
        
        //返回绑定类型的函数
        std::function<void()> handler(TriggerEvent event_type);
        //根据类型绑定函数
        void listen(TriggerEvent event_type, std::function<void()> callback);

        int getFd() const
        {
            return m_fd;
        }

        epoll_event getEpollEvent() const
        {
            return m_listen_events;
        }

    protected:
        int m_fd{-1};
        epoll_event m_listen_events;
        std::function<void()> m_read_callback;
        std::function<void()> m_write_callback;
    };

}

#endif