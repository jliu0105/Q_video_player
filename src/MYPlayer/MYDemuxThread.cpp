#include <iostream>
extern "C"
{
    #include <libavformat/avformat.h>
}
#include "MYDemux.h"
#include "MYVideoThread.h"
#include "MYAudioThread.h"
#include "MYDecode.h"
#include "MYDemuxThread.h"

#include <QDebug>


using namespace std;

void MYDemuxThread::Clear()
{
    mux.lock();
    if (demux)demux->Clear();
    if (vt)vt->Clear();
    if (at)at->Clear();
    mux.unlock();
}

void MYDemuxThread::Seek(double pos)
{
    Clear();
    mux.lock();
    bool status = this->is_pause;
    mux.unlock();

    change_pause(true);
    mux.lock();
    if (demux)
        demux->Seek(pos);
    mux.unlock();
    long long seekPts = pos*demux->totalMs;
    while (!is_exit)
    {
        AVPacket *pkt = demux->ReadVideo();
        if (!pkt) break;
        if (vt->pts_repaint(pkt, seekPts))
        {
            this->pts = seekPts;
            break;
        }

    }

    if (!status)
        change_pause(false);
}

void MYDemuxThread::change_pause(bool is_pause)
{
    mux.lock();
    this->is_pause = is_pause;
    if (at)at->change_pause(is_pause);
    if (vt)vt->change_pause(is_pause);
    mux.unlock();
}

void  MYDemuxThread::change_volume(double new_volume)
{
    if(!at)
    {
        return;
    }
    qDebug()<<new_volume;
    at->change_volume(new_volume);
}

void MYDemuxThread::setOpenSuccess(bool value)
{
    openSuccess = value;
}
bool MYDemuxThread::getOpenSuccess()
{
    return openSuccess;
}

void MYDemuxThread::run()
{
    while (!is_exit)
    {
        mux.lock();
        if (is_pause)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        if (!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        if (vt && at)
        {
            pts = at->pts;
            vt->synpts = at->pts;
        }

        AVPacket *pkt = demux->Read();
        if (!pkt)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        if (demux->IsAudio(pkt))
        {
            if (at)at->Push(pkt);
        }
        // video
        else 
        {
            if (vt)vt->Push(pkt);
        }
        mux.unlock();
        msleep(1);
    }
}

bool MYDemuxThread::Open(const char *url, IVideoCall *call)
{
    if (url == 0 || url[0] == '\0')
        return false;

    mux.lock();
    if (!demux) demux = new MYDemux();
    if (!vt) vt = new MYVideoThread();
    if (!at) at = new MYAudioThread();

    bool re = demux->Open(url);
    if (!re)
    {
        mux.unlock();
        cout << "demux->Open(url) failed!" << endl;
        return false;
    }
    setOpenSuccess(true);    
    if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
    {
        re = false;
        cout << "vt->Open failed!" << endl;
    }
    if (!at->Open(demux->CopyAPara(), demux->sample_ratio, demux->channels))
    {
        re = false;
        cout << "at->Open failed!" << endl;
    }
    totalMs = demux->totalMs;
    mux.unlock();
    cout << "MYDemuxThread::Open " << re << endl;
    return re;
}

void MYDemuxThread::Close()
{
    is_exit = true;
    wait();
    if (demux) demux->Close();
    if (vt) vt->Close();
    if (at) at->Close();
    mux.lock();
    QThread::quit();
    delete demux;
    delete vt;
    delete at;
    demux = NULL;
    vt = NULL;
    at = NULL;
    openSuccess = false;
    mux.unlock();
}

void MYDemuxThread::Start()
{
    mux.lock();
    if (!demux) demux = new MYDemux();
    if (!vt) vt = new MYVideoThread();
    if (!at) at = new MYAudioThread();
    QThread::start();
    if (vt)vt->start();
    if (at)at->start();
    mux.unlock();
}
MYDemuxThread::MYDemuxThread()
{
}


MYDemuxThread::~MYDemuxThread()
{
    is_exit = true;
    wait();
}
