#include "rocket/common/log.h"
#include "rocket/net/timer.h"
#include "rocket/common/util.h"
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <vector>
#include <string.h>

namespace rocket
{
    Timer::Timer() : FdEvent(0)
    {
        m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        DEBUGLOG("timer fd=%d", m_fd);

        // 把fd可读事件放到了eventloop上监听
        listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
    }

    Timer::~Timer()
    {
    }

    void Timer::addTimerEvent(TimerEvent::s_ptr event)
    {
        bool is_reset_timerfd = false;
        ScopeMutex<Mutex> lock(m_mutex);
        if (m_pending_events.empty())
        {
            is_reset_timerfd = true;
        }
        else
        {
            auto it = m_pending_events.begin();
            if ((*it).second->getArriveTime() > event->getArriveTime())
            {
                is_reset_timerfd = true;
            }
        }
        m_pending_events.emplace(event->getArriveTime(), event);
        lock.unlock();

        // 如果容器为空或新添加的事件的到达时间比当前最早的事件到达时间还早
        // 就需要重新设置定时器的触发时间
        if (is_reset_timerfd)
        {
            resetArriveTime();
        }
        DEBUGLOG("success addTimerEvent arrive time: %lld", event->getArriveTime());
    }

    // 重置定时器的触发时间
    void Timer::resetArriveTime()
    {
        ScopeMutex<Mutex> lock(m_mutex);
        auto tmp = m_pending_events;
        lock.unlock();
        if (tmp.empty())
        {
            return;
        }
        int64_t now = getNowMs();

        auto it = tmp.begin();
        int64_t interval = 0;
        if (it->second->getArriveTime() > now)
        {
            interval = it->second->getArriveTime() - now;
        }
        else
        { // 过期任务
            interval = 100;
        }

        // 新的interval
        timespec ts;
        memset(&ts, 0, sizeof(ts));
        ts.tv_sec = interval / 1000;
        ts.tv_nsec = (interval % 1000) * 1000000;
        itimerspec value;
        memset(&value, 0, sizeof(value));
        value.it_value = ts;

        int rt = timerfd_settime(m_fd, 0, &value, NULL); // 到达时间触发可读事件
        if (rt != 0)
        {
            ERRORLOG("timerfd_settime");
        }
        DEBUGLOG("timer reset to %lld", now + interval);
    }

    void Timer::deleteTimerEvent(TimerEvent::s_ptr event)
    {
        event->setCancled(true); // 先让他不可以执行
        ScopeMutex<Mutex> lock(m_mutex);
        auto begin = m_pending_events.lower_bound(event->getArriveTime()); // 有重复的
        auto end = m_pending_events.upper_bound(event->getArriveTime());
        // 在范围里找event
        auto it = begin;
        for (; it != end; ++it)
        {
            if (it->second == event)
            {
                break;
            }
        }
        if (it != end)
        {
            m_pending_events.erase(it);
        }
        lock.unlock();
        DEBUGLOG("success deleteTimerEvent arrive time: %lld", event->getArriveTime());
    }

    void Timer::onTimer()
    {
        // 处理缓冲区数据，防止下一次继续触发可读事件
        char buf[8];
        while (1)
        {
            if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN)
            {
                break;
            }
        }

        // 执行定时任务
        int64_t now = getNowMs();
        std::vector<TimerEvent::s_ptr> tmps;
        std::vector<std::pair<int64_t, std::function<void()>>> tasks;
        ScopeMutex<Mutex> lock(m_mutex);
        auto it = m_pending_events.begin();
        for (; it != m_pending_events.end(); ++it)
        {
            if ((*it).first <= now)
            {
                if (!(*it).second->isCancled())
                    tmps.push_back((*it).second);
                tasks.push_back(std::make_pair((*it).second->getArriveTime(), (*it).second->getCallBack()));
            }
            else
            {
                break;
            }
        }
        m_pending_events.erase(m_pending_events.begin(), it);
        lock.unlock();

        // 需要把重复的Eevnt 再次加进去
        for (auto i = tmps.begin(); i != tmps.end(); ++i)
        {
            if ((*i)->isRepeated())
            {
                (*i)->resetArriveTime();
                addTimerEvent(*i);
            }
        }
        resetArriveTime();
        for (auto i : tasks)
        {
            if (i.second)
            {
                i.second(); // 执行所有的定时任务
            }
        }
    }
}