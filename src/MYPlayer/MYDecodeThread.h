#ifndef MYDECODETHREAD_H
#define MYDECODETHREAD_H

// decode and show video 
#include <list>
#include <mutex>
#include <QThread>

struct AVPacket;
class MYDecode;

class MYDecodeThread :public QThread
{
public:
    MYDecodeThread();
    virtual ~MYDecodeThread();
    virtual void Push(AVPacket *pkt);
    virtual void Clear();
    virtual void Close();

    virtual AVPacket *Pop();
    int maxList = 100;
    bool is_exit = false;

protected:
    MYDecode *decode = 0;
    std::list <AVPacket *> packs;
    std::mutex mux;
};

#endif 
