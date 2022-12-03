#ifndef MYAUDIOPLAYTHREAD_H
#define MYAUDIOPLAYTHREAD_H

#include <mutex>
#include <list>
#include <QThread>
#include "MYDecodeThread.h"


struct AVCodecParameters;
class MYAudioPlay;
class MYResample;

class MYAudioThread : public MYDecodeThread
{
public:
    long long pts = 0;
    virtual bool Open(AVCodecParameters *para, int sample_ratio, int channels);

    virtual void Close();

    virtual void Clear();
    void run();
    MYAudioThread();
    virtual ~MYAudioThread();

    void change_pause(bool is_pause);
    bool is_pause = false;
    void change_volume(double new_volume);

protected:
    std::mutex amux;
    MYAudioPlay *ap = 0;
    MYResample *res = 0;

};

#endif // MYAUDIOPLAYTHREAD_H
