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




---

TimerEvent 定时任务
在网络框架中，不只有执行回调函数，还有定时任务
例如：调用请求，然后如果超过了5s没有返回，就返回一个错误，就需要一个定时器，判断这个任务5s有没有成功
如果到了5s任务没完成，就应该报错
我们有定时任务的需求

定时任务需要哪些东西
1. 指定的时间点 arrive_time
2. 间隔时间 inerval,ms
3. 是否是周期性任务 需要重复执行 is_repeaded
4. 取消标志 is_cancled
5. task

cancle() 取消任务
cancleRepeated() 取消周期执行 

定时器Timer
定时器 是一个TimerEvent的集合
Timer 继承 FdEvent

multimap 存储 TimerEvent< arrivetime, TimerEvent> 可重复的

addTimerEvent();
deleteTimerEvent();
onTimer();//当发生 IO 事件之后，需要执行的方法

reserArriveTimer()




