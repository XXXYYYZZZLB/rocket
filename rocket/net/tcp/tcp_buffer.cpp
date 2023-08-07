#include "rocket/net/tcp/tcp_buffer.h"
#include "rocket/common/log.h"
#include <memory>
#include <string.h>

/*
       readIndex      writeIndex
            |              |
        [ ][b][c][f][g][t][ ][ ][ ][ ][ ][ ][ ]
*/

namespace rocket
{
    TcpBuffer::TcpBuffer(int size) : m_size(size)
    {
        m_buffer.resize(size);
    }
    TcpBuffer::~TcpBuffer()
    {
    }

    int TcpBuffer::readAble()
    {
        // 可读是 writeIndex - readIndex
        return m_write_index - m_read_index;
    }

    int TcpBuffer::writeAble()
    {
        return m_buffer.size() - m_write_index;
    }

    int TcpBuffer::readIndex()
    {
        return m_read_index;
    }

    int TcpBuffer::writeIndex()
    {
        return m_write_index;
    }

    void TcpBuffer::writeToBuffer(const char *buf, int size)
    {
        if (size > writeAble())
        {
            DEBUGLOG("写入出错：需要写入的数据 大于 buffer可写数据,需要扩容");
            // 需要扩容
            int new_size = static_cast<int>(1.5 * (m_write_index + size));
            reSizeBuffer(new_size);
        }
        memcpy(&m_buffer[m_write_index], buf, size);
    }
    void TcpBuffer::readFromBuffer(std::vector<char> &re, int size)
    {
        if (readAble() == 0) // 没有可读的
        {
            re.clear();
            return;
        }
        int read_size = readAble() > size ? size : readAble();
        std::vector<char> tmp(read_size);
        memcpy(&tmp[0], &m_buffer[m_read_index], read_size);
        re.swap(tmp);
        m_read_index += read_size;

        adjustBuffer();
    }

    void TcpBuffer::reSizeBuffer(int new_size)
    {
        std::vector<char> tmp(new_size);
        int count = std::min(new_size, readAble());
        memcpy(&tmp[0], &m_buffer[m_read_index], count);
        m_buffer.swap(tmp);
        m_read_index = 0;
        m_write_index = m_read_index + count;
    }

    void TcpBuffer::adjustBuffer()
    {
        // 数组平移
        if (m_read_index < static_cast<int>(m_buffer.size() / 3))
        {
            return;
        }
        std::vector<char> buffer(m_buffer.size());
        int count = readAble();
        memcpy(&buffer[0], &m_buffer[m_read_index], count);
        m_buffer.swap(buffer);
        m_read_index = 0;
        m_write_index = m_read_index + count;
    }

    void TcpBuffer::moveReadIndex(int size)
    {
        size_t j = m_read_index + size;
        if(j>=m_buffer.size()){
            ERRORLOG("moveReadIndex 输入了非法的移动距离");
            return;
        }
        m_read_index = j;
        adjustBuffer();
    }
    void TcpBuffer::moveWriteIndex(int size)
    {
        size_t j = m_write_index + size;
        if(j>=m_buffer.size()){
            ERRORLOG("moveWriteIndex 输入了非法的移动距离");
            return;
        }
        m_write_index = j;
        adjustBuffer();
    }

} // namespace rocket
