#include "rocket/net/wakeup_fd_event.h"
#include "rocket/common/log.h"
#include <unistd.h>

namespace rocket
{
    WakeUpFdEvent::WakeUpFdEvent(int fd) : FdEvent(fd)
    {
    }

    WakeUpFdEvent::~WakeUpFdEvent()
    {
    }

    void WakeUpFdEvent::wakeup()
    {
        char buf[8] = {"a"};
        int rt = write(m_fd, buf, 8);
        if (rt != 8)
        {
            ERRORLOG("WakeUpFdEvent::wakeup 写入少于 8 bytes");
        }
        ERRORLOG("wakeup写入");
    }
    // 就是说，一旦设置了wakeup，
    // 就触发了epoll的读事件（因为写入了），
    // 这样就可以即使返回处理

}
