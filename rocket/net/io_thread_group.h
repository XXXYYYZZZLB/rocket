#ifndef ROCKER_NET_IO_THREAD_GROUP_H
#define ROCKER_NET_IO_THREAD_GROUP_H

// IO线程组（IO线程池）
#include <vector>
#include "rocket/common/log.h"
#include "rocket/net/io_thread.h"

namespace rocket
{
    class IOThreadGroup
    {

    public:
        IOThreadGroup(int size);
        ~IOThreadGroup();

        void start();
        void join();
        IOThread *getIOThread(); // 从当前线程组获取一个可用的线程

    private:
        int m_size;
        std::vector<IOThread *> m_io_thread_groups;

        // 用于轮询
        int m_index{0};
    };

} // namespace rocket

#endif