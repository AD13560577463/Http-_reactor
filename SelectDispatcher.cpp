#include "Dispatcher.h"
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include "SelectDispatcher.h"
//select四大函数
/*
* 将文件描述符fd从set集合中删除 == 将对应的标志位置0
* void FD_CLR(int fd,fd_set *set);
* 判断文件描述符fd是否在set集合中==读一下fd对应的标志位是0还是1
* int FD_ISSET(int fd,fd_set *set);
* 将文件描述符添加到set集合中==将对应的标志位置为1
* void FD_SET(int fd,fd_set *set)
* 将set集合中，所有文件描述符对应的标志位置为0
* void FD_ZERO(fd_set *set)
*/
SelectDispatcher::SelectDispatcher(EventLoop* evloop) :Dispatcher(evloop)
{
    //将读集合和写集合的缓冲区归零
    FD_ZERO(&m_readSet);
    FD_ZERO(&m_writeSet);
    m_name = "Select";
}

SelectDispatcher::~SelectDispatcher()
{
}

int SelectDispatcher::add()
{
    //获得的通信描述符不能超过最大的
    if (m_channel->getSocket() >= m_maxSize)
    {
        return -1;
    }
    setFdSet();//将文件描述符添加到集合中
    return 0;
}

int SelectDispatcher::remove()
{
    clearFdSet();
    // 通过 channel 释放对应的 TcpConnection 资源
    m_channel->destroyCallback(const_cast<void*>(m_channel->getArg()));

    return 0;
}

int SelectDispatcher::modify()
{
    setFdSet();
    clearFdSet();
    return 0;
}

int SelectDispatcher::dispatch(int timeout)
{
    struct timeval val;
    val.tv_sec = timeout;
    val.tv_usec = 0;
    fd_set rdtmp = m_readSet;
    fd_set wrtmp = m_writeSet;
    int count = select(m_maxSize, &rdtmp, &wrtmp, NULL, &val);
    if (count == -1)
    {
        perror("select");
        exit(0);
    }
    for (int i = 0; i < m_maxSize; ++i)
    {
        if (FD_ISSET(i, &rdtmp))
        {
            m_evLoop->eventActive(i, (int)FDEvent::ReadEvent);
        }

        if (FD_ISSET(i, &wrtmp))
        {
            m_evLoop->eventActive(i, (int)FDEvent::WriteEvent);
        }
    }
    return 0;
}

void SelectDispatcher::setFdSet()
{
    //将文件描述符添加到集合中
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        FD_SET(m_channel->getSocket(), &m_readSet);//将文件描述符添加到读集合去
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        FD_SET(m_channel->getSocket(), &m_writeSet);//将文件描述符添加到写集合去
    }
}

void SelectDispatcher::clearFdSet()
{
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        FD_CLR(m_channel->getSocket(), &m_readSet);
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        FD_CLR(m_channel->getSocket(), &m_writeSet);
    }
}
