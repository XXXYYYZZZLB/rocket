#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "rocket/net/eventloop.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"

#define ADD_TO_EPOLL()                                                                \
    auto it = m_listen_fds.find(event->getFd());                                      \
    int op = EPOLL_CTL_ADD;                                                           \
    if (it != m_listen_fds.end())                                                     \
    {                                                                                 \
        op = EPOLL_CTL_MOD;                                                           \
    }                                                                                 \
    epoll_event tmp = event->getEpollEvent();                                         \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                         \
    if (rt == -1)                                                                     \
    {                                                                                 \
        ERRORLOG("ADD_TO_EPOLL error! error info[%d], fd=%d", errno, event->getFd()); \
    }                                                                                 \
    DEBUGLOG("add eent success,fd[%d]", event->getFd());

#define DELETE_TO_EPOLL()                                                                \
    auto it = m_listen_fds.find(event->getFd());                                         \
    if (it != m_listen_fds.end())                                                        \
    {                                                                                    \
        return;                                                                          \
    }                                                                                    \
    int op = EPOLL_CTL_DEL;                                                              \
    epoll_event tmp = event->getEpollEvent();                                            \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                            \
    if (rt == -1)                                                                        \
    {                                                                                    \
        ERRORLOG("DELETE_TO_EPOLL error! error info[%d], fd=%d", errno, event->getFd()); \
    }                                                                                    \
    DEBUGLOG("add eent success,fd[%d]", event->getFd());

namespace rocket
{

    static thread_local EventLoop *t_current_eevent = NULL;
    // 有且只有 thread_local 关键字修饰的变量具有线程（thread）周期
    // 这些变量在线程开始的时候被生成，在线程结束的时候被销毁，并且每一个线程都拥有一个独立的变量实例
    static int g_epoll_max_timeout = 10000;
    static int g_epoll_max_events = 10;

    EventLoop::EventLoop()
    {
        if (t_current_eevent != NULL)
        {
            ERRORLOG("failed to create event loop, this thread has created event loop\n");
            exit(0);
        }
        m_thread_id = getThreadId();
        m_epoll_fd = epoll_create(10);
        if (m_epoll_fd == -1)
        {
            ERRORLOG("epoll_create error! error info[%d]", errno);
            exit(0);
        }

        m_wakeup_fd = eventfd(0, EFD_NONBLOCK); // 非阻塞的
        if (m_wakeup_fd < 0)
        {
            ERRORLOG("eventfd error! error info[%d]", errno);
            exit(0);
        }

        initWakeUpFdEvent();
        INFOLOG("succ create event loop in thread %d", m_thread_id);
        t_current_eevent = this;
    }
    EventLoop::~EventLoop()
    {
        close(m_epoll_fd);
        if(m_wakeup_fd_event){
            delete m_wakeup_fd_event;
            m_wakeup_fd_event = NULL;
        }
    }

    void EventLoop::loop()
    {
        while (!m_stop_flag)
        {
            ScopeMutex<Mutex> lock(m_mutex);
            std::queue<std::function<void()>> tmp_tasks = m_pending_tasks;
            m_pending_tasks.swap(tmp_tasks);
            lock.unlock();

            while (!tmp_tasks.empty()) // 执行队列所有的任务
            {
                tmp_tasks.front()();
                tmp_tasks.pop();
            }

            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];
            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);

            if (rt < 0)
            {
                ERRORLOG("epoll_wait error,errno=", errno);
            }
            else
            {
                for (int i = 0; i < rt; ++i)
                {
                    epoll_event trigger_event = result_events[i];
                    FdEvent *fd_event = static_cast<FdEvent *>(trigger_event.data.ptr);
                    if (fd_event == NULL)
                    {
                        continue;
                    }
                    if (trigger_event.events | EPOLLIN)
                    {
                        addTask(fd_event->handler(FdEvent::IN_EVENT));
                    }
                    if (trigger_event.events | EPOLLOUT)
                    {
                        addTask(fd_event->handler(FdEvent::OUT_EVENT));
                    }
                }
            }
        }
    }

    void EventLoop::wakeup()
    {
    }

    void EventLoop::stop()
    {
        m_stop_flag = true;
    }

    void EventLoop::addEpollEvent(FdEvent *event)
    {
        if (isInLoopThread)
        {
            ADD_TO_EPOLL();
        }
        else
        {
            auto cb = [=]()
            {
                ADD_TO_EPOLL();
            };
            addTask(cb, true);
        }
    }
    void EventLoop::deleteEpollEvent(FdEvent *event)
    {
        if (isInLoopThread())
        {
            DELETE_TO_EPOLL();
        }
        else
        {
            auto cb = [=]()
            {
                DELETE_TO_EPOLL();
            };
            addTask(cb, true);
        }
    }

    bool EventLoop::isInLoopThread()
    {
        return getThreadId() == m_thread_id;
    }

    void EventLoop::addTask(std::function<void()> cb, bool is_wake_up /*=false*/)
    {
        ScopeMutex<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();
        if (is_wake_up)
        {
            wakeup();
        }
    }

    void EventLoop::initWakeUpFdEvent()
    {
        m_wakeup_fd_event = new WakeUpFdEvent(m_wakeup_fd);
        m_wakeup_fd_event->listen(FdEvent::IN_EVENT, [=]()
                                  {
            char buf[8];
            while (read(m_wakeup_fd,buf,8)!=-1 && errno != EAGAIN)
            {
            }
            DEBUGLOG("read full bytes"); });

        addEpollEvent(m_wakeup_fd_event);
    }

}
