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


IO线程
创建一个IO线程，他会帮我们执行
1. 创建一个新线程
2. 在新线程里 创建一个EventLoop,完成初始化
3. loop循环

class{
    pthread_t m_thread; //线程指针
    pid_t m_thread_id;  //线程id
    Eventloop eventloop;
}





# Main
IOThreadGroup是一个IO线程数组
主要成员变量为：
    std::vector<IOThread *> m_io_thread_groups;


IOThread是一个IO线程类
主要成员变量为：
    pthread_t m_thread{0};        // 线程句柄
    EventLoop *m_event_loop{NULL}; // 当前io线程的loop对象
主要功能为：
    创建一个线程（将this指针传入），在新线程中创建IOThread对象
    初始化EventLoop、设置m_thread_id
    等待唤醒，开始loop循环


EventLoop类
主要成员变量为：
    pid_t m_thread_id{0}; // 记录线程id
    int m_epoll_fd{0};
    int m_wakeup_fd{0};
    WakeUpFdEvent *m_wakeup_fd_event{NULL};
    bool m_stop_flag{false};
    std::set< int> m_listen_fds;                         // 监听的套接字
    std::queue< std::function< void()>> m_pending_tasks; // 所有待执行任务的队列
    Mutex m_mutex;
    Timer *m_timer{NULL};
主要功能为：
    EventLoop需要先创建对象 -> 然后添加监听addTimerEvent、void addEpollEvent -> 最后进入loop循环
    1.EventLoop需要先创建对象：
    先记录当前线程的id，EventLoop都是创建在非主线程中的
    创建epoll文件监听描述符
    创建m_wakeup_fd通知文件描述符(eventfd)
    initWakeUpFdEvent() 及时响应事件
        将m_wakeup_fd包装为WakeUpFdEvent(m_wakeup_fd_event)
        使用listen设置 m_wakeup_fd的回调函数 read 8个字节(因为触发是write 8个字节)
        addEpollEvent() 将m_wakeup_fd_event添加到epoll监听中
        (判断是否是IO线程(根据m_thread_id)，是就直接添加，不是就添加到任务队列)
    initTimer() 定时事件
        m_timer = new Timer();
        addEpollEvent(m_timer);
    2.然后添加监听addTimerEvent、void addEpollEvent
    3.最后进入loop循环
    锁住m_pending_tasks，执行内置的所有任务
    阻塞到epoll_wait,在上述任务触发之前会一直阻塞在这里
    解除阻塞以后根据event添加任务，trigger_event.data.ptr保存了对象指针
    然乎回到开头执行任务，阻塞...

Timer类
主要成员变量为：
    int64_t m_arrive_time; // ms
    int64_t m_interval;    // ms
    bool m_is_repeated{false};
    bool m_is_cancled{false};
    std::function< void()> m_task;//定时的任务


Timer类继承自FdEvent
主要成员变量为：
    std::multimap< int64_t, TimerEvent::s_ptr> m_pending_events;
主要功能为：
    1.timerfd_create创建m_fd,listen设置 m_fd的回调函数是onTimer();
    2.addTimerEvent
        m_pending_events中添加event
    3.如果容器为空或新添加的事件的到达时间比当前最早的事件到达时间还早 就需要重新设置定时器的触发时间
        更新时间，然后使用timerfd_settime来设置m_fd最小的间隔时间，因为m_fd是来触发epoll_wait的
    关于onTimer
        先处理缓冲区数据，防止下一次继续触发可读事件(可以不处理，其实没有写入)
        执行定时任务，把重复的再重新存储进去


    
    
    




