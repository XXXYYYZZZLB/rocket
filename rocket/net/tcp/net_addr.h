#ifndef ROCKET_NET_TCP_NET_ADDR_H
#define ROCKET_NET_TCP_NET_ADDR_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>

namespace rocket
{
    class NetAddr // 基类
    {
    public:
        typedef std::shared_ptr<NetAddr> s_ptr; // 为了多态的实现
        virtual sockaddr *getSockAddr() = 0;    // 纯虚函数，让子类去实现
        virtual socklen_t getSockLen() = 0;
        virtual int getFamily() = 0;        // 协议族
        virtual std::string toString() = 0; // 返回点分十进制

        virtual bool checkValid() = 0;
    };

    class IPNetaddr : public NetAddr
    {
    public:
        IPNetaddr(const std::string &ip, uint16_t port);
        IPNetaddr(const std::string &addr); // 点分十进制
        IPNetaddr(sockaddr_in addr);        // 点分十进制

        sockaddr *getSockAddr();
        socklen_t getSockLen();
        int getFamily();
        std::string toString();
        bool checkValid();

    private:
        uint16_t m_port;
        std::string m_ip{0};
        sockaddr_in m_addr;
    };
} // namespace rocket

#endif