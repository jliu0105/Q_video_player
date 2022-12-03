#include <iostream>
extern "C"
{
    #include <libavformat/avformat.h>
}
#include "MYDemux.h"


using namespace std;

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

bool MYDemux::Open(const char *url)
{
    Close();
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    av_dict_set(&opts, "max_delay", "500", 0);

    mux.lock();
    int re = avformat_open_input(
        &ic,
        url,
        0,        
        &opts   
    );
    if (re != 0)
    {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open " << url << " failed! :" << buf << endl;
        return false;
    }
    cout << "open " << url << " success! " << endl;

    re = avformat_find_stream_info(ic, 0);

    this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    // print video info detail 
    av_dump_format(ic, 0, url, 0);


    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *as = ic->streams[videoStream];
    width = as->codecpar->width;
    height = as->codecpar->height;

    cout << "======================================================================" << endl;
    cout << videoStream << "Video format" << endl;
    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << "width=" << as->codecpar->width << endl;
    cout << "height=" << as->codecpar->height << endl;
    cout << "video fps = " << r2d(as->avg_frame_rate) << endl;

    cout << "======================================================================" << endl;
    cout << audioStream << "sound info" << endl;
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    as = ic->streams[audioStream];
    sample_ratio = as->codecpar->sample_rate;
    channels = as->codecpar->channels;

    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << "sample_rate = " << as->codecpar->sample_rate << endl;
    cout << "channels = " << as->codecpar->channels << endl;
    cout << "frame_size = " << as->codecpar->frame_size << endl;
    mux.unlock();


    return true;
}

void MYDemux::Clear()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_flush(ic);
    mux.unlock();
}
void MYDemux::Close()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_close_input(&ic);
    totalMs = 0;
    mux.unlock();
}

bool MYDemux::Seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if (re < 0) return false;
    return true;
}

AVCodecParameters *MYDemux::CopyVPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mux.unlock();
    return pa;
}

AVCodecParameters *MYDemux::CopyAPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
    mux.unlock();
    return pa;
}
bool MYDemux::IsAudio(AVPacket *pkt)
{
    if (!pkt) return false;
    if (pkt->stream_index == videoStream)
        return false;
    return true;

}

AVPacket *MYDemux::ReadVideo()
{
    mux.lock();
    if (!ic) 
    {
        mux.unlock();
        return 0;
    }
    mux.unlock();
    AVPacket *pkt = NULL;
    for (int i = 0; i < 20; i++)
    {
        pkt = Read();
        if (!pkt)break;
        if (pkt->stream_index == videoStream)
        {
            break;
        }
        av_packet_free(&pkt);
    }
    return pkt;
}

AVPacket *MYDemux::Read()
{
    mux.lock();
    if (!ic) 
    {
        mux.unlock();
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return 0;
    }
    pkt->pts = pkt->pts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    mux.unlock();
    return pkt;

}
MYDemux::MYDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if (isFirst)
    {
        av_register_all();

        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}


MYDemux::~MYDemux()
{
}