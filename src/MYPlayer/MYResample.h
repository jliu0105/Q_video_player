#ifndef MYRESAMPLE_H
#define MYRESAMPLE_H

#include <mutex>


struct AVCodecParameters;
struct AVFrame;
struct SwrContext;

class MYResample
{
public:
    virtual bool Open(AVCodecParameters *para, bool isClearPara = false);
    virtual void Close();

    virtual int sample_again(AVFrame *indata, unsigned char *data);

    MYResample();
    virtual ~MYResample();

    int output_format = 1;
protected:
    std::mutex mux;
    SwrContext *actx = 0;
};

#endif 
