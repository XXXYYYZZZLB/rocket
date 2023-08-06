#ifndef ROCKET_NET_IO_THREAD_H
#define ROCKET_NET_IO_THREAD_H

#include "rocket/net/eventloop.h"
#include <pthread.h>
#include <semaphore.h> //用来做同步用

namespace rocket
{
    class IOThread
    {
    public:
        IOThread();
        ~IOThread();

        EventLoop *getEventLoop();
        void start();
        void jion();

    public:
        // 线程执行的函数
        static void *Main(void *arg); // 注意！必须是静态函数 静态函数用大写

    private:
        pid_t m_thread_id{-1};         // 线程号
        pthread_t m_thread{0};        // 线程句柄
        EventLoop *m_event_loop{NULL}; // 当前io线程的loop对象

        sem_t m_init_semaphore; // 用来做同步用,完成线程前置部分
        sem_t m_start_semaphore;
    };

}

#endif