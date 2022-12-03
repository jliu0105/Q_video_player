#ifndef MYDEMUXTHREAD_H
#define MYDEMUXTHREAD_H

#include <mutex>
#include <QThread>
#include "IVideoCall.h"


class MYDemux;
class MYVideoThread;
class MYAudioThread;

class MYDemuxThread :public QThread
{
public:
    virtual bool Open(const char *url, IVideoCall *call);
    virtual void Start();

    virtual void Close();
    virtual void Clear();

    virtual void Seek(double pos);

    void run();
    MYDemuxThread();
    virtual ~MYDemuxThread();
    bool is_exit = false;
    long long pts = 0;
    long long totalMs = 0;
    void change_pause(bool is_pause);
    bool is_pause = false;

    void change_volume(double new_volume);

    void setOpenSuccess(bool value);
    bool getOpenSuccess();
protected:
    std::mutex mux;
    MYDemux *demux = 0;
    MYVideoThread *vt = 0;
    MYAudioThread *at = 0;
    bool openSuccess = false;
};

#endif 
