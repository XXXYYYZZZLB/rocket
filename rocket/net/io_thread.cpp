#include <pthread.h>
#include "rocket/net/io_thread.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"
#include <assert.h>

namespace rocket
{

    IOThread::IOThread()
    {
        // 初始化信号量
        int rt = sem_init(&m_init_semaphore, 0, 0);
        assert(rt == 0);
        rt = sem_init(&m_start_semaphore, 0, 0);
        assert(rt == 0);

        // 在构造函数中创建一个线程
        pthread_create(&m_thread, NULL, &IOThread::Main, this);

        // wait 一直等到，直到main函数新线程执行完前置
        // 才可以往下走
        sem_wait(&m_init_semaphore);
        // 会一直等到m_init_semaphore大于0

        // 此时的Main前置已经执行完毕了
        DEBUGLOG("IO线程 创建完毕 线程号：%d", m_thread_id);
    }

    IOThread::~IOThread()
    {
        m_event_loop->stop();
        sem_destroy(&m_init_semaphore);
        sem_destroy(&m_start_semaphore);

        pthread_join(m_thread, NULL);
        if (m_event_loop)
        {
            delete m_event_loop;
            m_event_loop = NULL;
        }
    }

    void *IOThread::Main(void *arg)
    {
        // 这是新线程
        IOThread *thread = static_cast<IOThread *>(arg);
        thread->m_event_loop = new EventLoop();
        thread->m_thread_id = getThreadId();

        // 在此处 唤醒等待的线程
        sem_post(&thread->m_init_semaphore);

        // 开始循环
        sem_wait(&thread->m_start_semaphore); // 等待触发
        thread->m_event_loop->loop();

        return NULL;
    }

    EventLoop *IOThread::getEventLoop()
    {
        return m_event_loop;
    }

    void IOThread::start()
    {
        sem_post(&m_start_semaphore);
        DEBUGLOG("现在 调用 IOThread %d", m_thread_id);
    }

    void IOThread::jion()
    {
        pthread_join(m_thread, NULL);
    }
}