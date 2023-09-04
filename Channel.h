#pragma once
#include <functional>

// 定义函数指针
// typedef int(*handleFunc)(void* arg);c语言的做法
// using handleFunc = int(*)(void*);c++的做法

// 定义文件描述符的读写事件
enum class FDEvent      //强制类型转换为class
{
    TimeOut = 0x01,
    ReadEvent = 0x02,
    WriteEvent = 0x04
};

// 可调用对象包装器打包的是什么? 1. 函数指针 2. 可调用对象(可以向函数一样使用)
// 最终得到了地址, 但是没有调用
//EventLoop内部需要的文件描述符Channel，有监听的有通信的，可以说是任务
class Channel
{
public:
    using handleFunc = std::function<int(void*)>;
    Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg);//赋值构造函数
    // 回调函数
    handleFunc readCallback;
    handleFunc writeCallback;
    handleFunc destroyCallback;
    // 修改fd的写事件(检测 or 不检测)
    void writeEventEnable(bool flag);
    // 判断是否需要检测文件描述符的写事件
    bool isWriteEventEnable();
    // 取出私有成员的值，内联函数适用于代码较少的情况，实现简单的代码替换
    inline int getEvent()
    {
        return m_events;
    }
    inline int getSocket()
    {
        return m_fd;
    }
    inline const void* getArg()
    {
        return m_arg;
    }
private:
    // 文件描述符
    int m_fd;
    // 事件
    int m_events;
    // 回调函数的参数
    void* m_arg;
};

