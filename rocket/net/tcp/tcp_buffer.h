#ifndef ROCKET_NET_TCP_TCP_BUFFRT_H
#define ROCKET_NET_TCP_TCP_BUFFRT_H


#include <vector>

namespace rocket
{

    class TcpBuffer
    {
        TcpBuffer(int size);
        ~TcpBuffer();

        //返回可读字节数
        int readAble();

        //返回可写字节数
        int writeAble();

        int readIndex();
        int writeIndex();

        void writeToBuffer(const char* buf,int size);
        void readFromBuffer(std::vector<char>& re, int size);

        void reSizeBuffer(int new_size);

        void adjustBuffer();


        //手动调整
        void moveReadIndex(int size);
        void moveWriteIndex(int size);

    private:
        std::vector<char> m_buffer;

        int m_read_index{0};
        int m_write_index{0};
        int m_size{0};
    };

} // namespace rocket

#endif