#include "rocket/common/util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>

namespace rocket
{

    static int g_pid = 0;
    static thread_local int g_thread_id = 0;

    pid_t getPid()
    {
        if (g_pid != 0)
        {
            return g_pid;
        }
        return getpid();
    }

    pid_t getThreadId()
    {
        if (g_thread_id != 0)
        {
            return g_thread_id;
        }
        return syscall(SYS_gettid);
    }

    /**
    函数使用了timeval 结构体，其中 tv_sec 表示秒数，tv_usec 表示微秒数。
    首先，将秒数 tv_sec 转换为 int64_t 类型，然后乘以1000，得到秒数的毫秒表示。
    接着，将微秒数 tv_usec 除以1000，得到微秒数的毫秒表示，再将这两部分相加得到总的毫秒数。
    使用这个函数，可以获取当前的毫秒级时间戳，用于计算时间间隔或进行其他时间相关的操作。
    */
    int64_t getNowMs()
    {
        timeval val;
        gettimeofday(&val, NULL);
        return val.tv_sec * 1000 + val.tv_usec / 1000;//毫秒数
    }

}