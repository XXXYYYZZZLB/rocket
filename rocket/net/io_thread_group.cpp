#include "rocket/net/io_thread_group.h"
#include "rocket/common/log.h"

// 只有主线程会调用这个，不需要成员加锁了，不用考虑线程安全

namespace rocket
{
    IOTHreadGroup::IOTHreadGroup(int size) : m_size(size)
    {
        m_io_thread_groups.resize(size);
        for (int i = 0; i < size; ++i)
        {
            m_io_thread_groups[i] = new IOThread();
        }
    }

    IOTHreadGroup::~IOTHreadGroup()
    {
    }

    void IOTHreadGroup::start()
    {
        for (int i = 0; i < m_io_thread_groups.size(); ++i)
        {
            m_io_thread_groups[i]->start();
        }
    }

    IOThread *IOTHreadGroup::getIOThread()
    {
        if (m_index == m_io_thread_groups.size() || m_index == -1)
        {
            m_index = 0;
        }
        return m_io_thread_groups[m_index++];
    }

    void IOTHreadGroup::join()
    {
        for (int i = 0; i < m_io_thread_groups.size(); ++i)
        {
            m_io_thread_groups[i]->jion();
        }
    }

}