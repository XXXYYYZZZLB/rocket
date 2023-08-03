Reactor模式

Reactor模式是事件驱动模型，有一个或多个并发输入源，有一个Service Handler，有多个Request Handlers；
这个Service Handler会同步的将输入的请求（Event）多路复用的分发给相应的Request Handler。
从结构上，这有点类似生产者消费者模式，即有一个或多个生产者将事件放入一个Queue中，而一个或多个消费者主动的从这个Queue中Poll事件来处理；
而Reactor模式则并没有Queue来做缓冲，每当一个Event输入到Service Handler之后，该Service Handler会主动的根据不同的Event类型将其分发给对应的Request Handler来处理。

FdEvent类
封装了文件描述符和对应的回调函数
    int m_fd{-1};
    epoll_event m_listen_events;
    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
listen：用于根据监听类型来设置回调函数
handler：用于根据监听类型来返回对应的回调函数

WakeUpFdEvent类(继承自FdEvent)
wakeup：向自己对应的文件描述符 write 8个字节

EventLoop类
    pid_t m_thread_id{0}; // 线程id
    int m_epoll_fd{0};
    int m_wakeup_fd{0};
    WakeUpFdEvent *m_wakeup_fd_event{NULL};
    bool m_stop_flag{false};
    std::set<int> m_listen_fds;                        // 监听的套接字
    std::queue<std::function<void()>> m_pending_tasks; // 所有待执行任务的队列
    Mutex m_mutex;
构造函数：
    记录IO线程id保存到类中
    创建epoll文件监听描述符保存到类中
    创建m_wakeup_fd通知文件描述符(eventfd)
    initWakeUpFdEvent()
        将m_wakeup_fd包装为WakeUpFdEvent(m_wakeup_fd_event)
        使用listen设置m_wakeup_fd的回调函数 read 8个字节
    addEpollEvent()
        将m_wakeup_fd_event添加到epoll监听中
        (判断是否是IO线程，是就直接添加，不是就添加到任务队列)

addTask：
    将任务处理函数添加进m_pending_tasks队列  (加锁)
    (如果设置了 wakeup()就执行)
wakeup：
    ...
loop：一直循环
    取出任务m_pending_tasks (加锁)
    执行队列的所有任务
    epoll_wait
        根据监听文件描述符
        ptr保存了FdEvent
        根据events，addTask



EventLoop流程


