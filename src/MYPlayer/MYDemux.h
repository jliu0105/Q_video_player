#ifndef MYDEMUX_H
#define MYDEMUX_H

#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class MYDemux
{
public:

    virtual bool Open(const char *url);

    virtual AVPacket *Read();

    virtual AVPacket *ReadVideo();

    virtual bool IsAudio(AVPacket *pkt);

    // get video parameter
    virtual AVCodecParameters *CopyVPara();

    // get audio parameter
    virtual AVCodecParameters *CopyAPara();

    //seek location pos 0.0~1.0
    virtual bool Seek(double pos);

    virtual void Clear();
    virtual void Close();


    MYDemux();
    virtual ~MYDemux();

    int totalMs = 0;
    int width = 0;
    int height = 0;
    int sample_ratio = 0;
    int channels = 0;



protected:
    std::mutex mux;
    AVFormatContext *ic = NULL;
    int videoStream = 0;
    int audioStream = 1;

};

#endif // MYDEMUX_H
