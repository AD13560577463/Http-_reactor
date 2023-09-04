#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include <string>
using namespace std;
class EventLoop;
//Dispathcer是分发器，用来实现选择epoll，poll，select中的一个，并且里面有什么任务
class Dispatcher
{
public:
    Dispatcher(EventLoop* evloop);//构造函数
    virtual ~Dispatcher();//为虚析构函数，就可以告诉内存释放掉子类对象
    // 添加
    virtual int add();
    // 删除
    virtual int remove();
    // 修改
    virtual int modify();
    // 事件监测
    virtual int dispatch(int timeout = 2); // 单位: s
    inline void setChannel(Channel* channel)//内联函数
    {
        m_channel = channel;//调用那个文件描述符，有监听的有通信的，或者说调用那个任务
    }
    protected:
    string m_name = string();
    Channel* m_channel;//从Channel中找到调用的文件描述符
    EventLoop* m_evLoop;//记录从EventLoop使用poll，select，epoll中的一个
};