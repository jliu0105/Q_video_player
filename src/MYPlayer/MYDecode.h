#ifndef MYDECODE_H
#define MYDECODE_H

#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
extern void MYFreePacket(AVPacket **pkt);
extern void MYFreeFrame(AVFrame **frame);

class MYDecode
{
public:
    bool isAudio = false;

    long long pts = 0;
    virtual bool Open(AVCodecParameters *para);

    virtual bool Send(AVPacket *pkt);

    virtual AVFrame* Recv();

    virtual void Close();
    virtual void Clear();

    MYDecode();
    virtual ~MYDecode();
protected:
    AVCodecContext *codec = 0;
    std::mutex mux;
};

#endif 
