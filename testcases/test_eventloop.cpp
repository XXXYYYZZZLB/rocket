#include <pthread.h>
#include "rocket/common/config.h"
#include "rocket/common/log.h"
#include "rocket/net/fd_event.h"
#include "rocket/net/wakeup_fd_event.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/timer.h"
#include "rocket/net/timer_event.h"
#include "rocket/net/io_thread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <memory>

void test_io_thread()
{

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        ERRORLOG("socket");
        exit(0);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(12345);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int mw_optval = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&mw_optval), sizeof(mw_optval));

    int rt = bind(listenfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (rt != 0)
    {
        ERRORLOG("bind");
        exit(0);
    }

    rt = listen(listenfd, 100);
    if (rt != 0)
    {
        ERRORLOG("listen");
        exit(0);
    }
    // listenfd创建好了

    rocket::FdEvent event(listenfd);
    event.listen(rocket::FdEvent::IN_EVENT, [listenfd]()
                 {
                     sockaddr_in peer_addr;
                     memset(&peer_addr, 0, sizeof(peer_addr));
                     socklen_t addr_len = sizeof(peer_addr);
                     int clientfd = accept(listenfd, reinterpret_cast<sockaddr *>(&peer_addr), &addr_len);

                     DEBUGLOG("listenfd 得到连接 succsee get client[%s:%d]", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)); }); // 监听可读事件

    int i = 0;
    rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
        1000, true, [&i]()
        { INFOLOG("trgger 定时任务 event, i = %d", i++); });

    rocket::IOThread io_thread;
    io_thread.getEventLoop()->addEpollEvent(&event);
    io_thread.getEventLoop()->addTimerEvent(timer_event);
    io_thread.start();

    io_thread.jion();
}

int main()
{

    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::InitGlobalLogger();

    test_io_thread();


    //rocket::EventLoop *eventloop = new rocket::EventLoop(); // 创建一个EventLoop循环

    // int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (listenfd == -1)
    // {
    //     ERRORLOG("socket");
    //     exit(0);
    // }

    // sockaddr_in addr;
    // memset(&addr, 0, sizeof(addr));

    // addr.sin_port = htons(12345);
    // addr.sin_family = AF_INET;
    // inet_aton("127.0.0.1", &addr.sin_addr);

    // int mw_optval = 1;
    // setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&mw_optval), sizeof(mw_optval));

    // int rt = bind(listenfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    // if (rt != 0)
    // {
    //     ERRORLOG("bind");
    //     exit(0);
    // }

    // rt = listen(listenfd, 100);
    // if (rt != 0)
    // {
    //     ERRORLOG("listen");
    //     exit(0);
    // }
    // // listenfd创建好了

    // rocket::FdEvent event(listenfd);
    // event.listen(rocket::FdEvent::IN_EVENT, [listenfd]()
    //              {
    //                  sockaddr_in peer_addr;
    //                  memset(&peer_addr, 0, sizeof(peer_addr));
    //                  socklen_t addr_len = sizeof(peer_addr);
    //                  int clientfd = accept(listenfd, reinterpret_cast<sockaddr *>(&peer_addr), &addr_len);

    //                  DEBUGLOG("listenfd 得到连接 succsee get client[%s:%d]", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)); }); // 监听可读事件
    // eventloop->addEpollEvent(&event);

    // int i = 0;
    // rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
    //     1000, true, [&i]()
    //     { INFOLOG("trgger timer event, i = %d", i++); });

    // eventloop->addTimerEvent(timer_event);

    // eventloop->loop();

    return 0;
}