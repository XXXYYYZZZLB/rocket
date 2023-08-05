#ifndef ROCKET_COMMON_UTIL_H
#define ROCKET_COMMON_UTIL_H

#include <sys/types.h>
#include <unistd.h>

namespace rocket
{
    pid_t getPid();
    pid_t getThreadId();

    //获取当前毫秒时间戳
    int64_t getNowMs();

}

#endif