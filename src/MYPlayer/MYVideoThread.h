#ifndef MYVIDEOTHREAD_H
#define MYVIDEOTHREAD_H

#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "MYDecodeThread.h"


struct AVPacket;
struct AVCodecParameters;
class MYDecode;

class MYVideoThread :public MYDecodeThread
{
public:
    virtual bool pts_repaint(AVPacket *pkt, long long seekpts);
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);

    void run();

    MYVideoThread();
    virtual ~MYVideoThread();

    long long synpts = 0;

    void change_pause(bool is_pause);
    bool is_pause = false;


protected:
    IVideoCall *call = 0;
    std::mutex vmux;


};

#endif // MYVIDEOTHREAD_H
