#include "rocket/common/log.h"
#include "rocket/common/config.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_server.h"
#include <memory>

void test_tcp_server()
{
    rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1:12345");

    DEBUGLOG("创建连接:%s", addr->toString().c_str());

    rocket::TcpServer tcp_server(addr);

    tcp_server.start();
}

int main()
{

    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::InitGlobalLogger();

    test_tcp_server();

    return 0;
}