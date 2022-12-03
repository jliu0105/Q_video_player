#include <iostream>
extern "C"
{
    #include <libswresample/swresample.h>
    #include <libavcodec/avcodec.h>
}
#include "MYResample.h"


using namespace std;

void MYResample::Close()
{
    mux.lock();
    if (actx)
        swr_free(&actx);

    mux.unlock();
}

bool MYResample::Open(AVCodecParameters *para, bool isClearPara)
{
    if (!para)return false;
    mux.lock();


    //如果actx为NULL，会自动分配空间
    // if actx is null, distribute space automatically 
    actx = swr_alloc_set_opts(actx,
        av_get_default_channel_layout(2),	//export style
        (AVSampleFormat)output_format,			//export AV_SAMPLE_FMT_S16
        para->sample_rate,					//export sample rate
        av_get_default_channel_layout(para->channels),// export format 
        (AVSampleFormat)para->format,
        para->sample_rate,
        0, 0
    );
    if (isClearPara)
        avcodec_parameters_free(&para);
    int re = swr_init(actx);
    mux.unlock();
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init  failed! :" << buf << endl;
        return false;
    }
    //unsigned char *pcm = NULL;
    return true;
}

int MYResample::sample_again(AVFrame *indata, unsigned char *d)
{
    if (!indata) return 0;
    if (!d)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *data[2] = { 0 };
    data[0] = d;
    int re = swr_convert(actx,
        data, indata->nb_samples,		// output
        (const uint8_t**)indata->data, indata->nb_samples	// input 
    );
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)output_format);
    av_frame_free(&indata);
    if (re <= 0)return re;
    return outSize;
}
MYResample::MYResample()
{
}


MYResample::~MYResample()
{
}
