#include "rocket/common/log.h"
#include "rocket/common/config.h"
#include "rocket/net/tcp/net_addr.h"

int main()
{

    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::InitGlobalLogger();

    rocket::IPNetaddr addr("127.0.0.1:9999");
    DEBUGLOG("创建连接:%s", addr.toString().c_str());

    return 0;
}