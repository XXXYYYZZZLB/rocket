#include "rocket/net/wakeup_fd_event.h"
#include "rocket/common/log.h"
#include <unistd.h>

namespace rocket
{
    WakeUpFdEvent::WakeUpFdEvent(int fd) : FdEvent(fd) {
        init();
    }

    WakeUpFdEvent::~WakeUpFdEvent()
    {
    }

    void WakeUpFdEvent::init()
    {
        m_read_callback = [=]()
        {
            char buf[8];
            while (read(m_fd, buf, 8) != -1 && errno != EAGAIN)
            {

            }
            DEBUGLOG("read full bytes");
        };
    }

    void WakeUpFdEvent::wakeup()
    {
        char buf[8] = {"a"};
        int rt = write(m_fd, buf, 8);
        if (rt != 8)
        {
            ERRORLOG("write to wakeup less 8bytes");
        }
    }
}
