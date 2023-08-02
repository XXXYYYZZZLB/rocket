#include "rocket/common/log.h"
#include "rocket/common/config.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

void *func(void *)
{
    int i = 20;
    while (i--)
    {
        DEBUGLOG("debug this is thread in %s", "22");
        INFOLOG("info this is thread in %s", "22");
    }

    return NULL;
}

int main()
{
    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::InitGlobalLogger();

    pthread_t thread;
    pthread_create(&thread, NULL, &func, NULL);

    int i = 20;
    while (i--)
    {
        DEBUGLOG("main test debug %d", i);
        INFOLOG("main test info %d", i);
    }

    pthread_join(thread, NULL);
    printf("end\n");

    return 0;
}