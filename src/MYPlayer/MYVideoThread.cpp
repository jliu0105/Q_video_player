#include <iostream>
#include "MYVideoThread.h"
#include "MYDecode.h"

using namespace std;

// open, clean no matter successful or not 
bool MYVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para)return false;

    Clear();

    vmux.lock();
    synpts = 0;
    // initialize showing window 
    this->call = call;
    if (call)
    {}
    vmux.unlock();
    int re = true;
    if (!decode->Open(para))
    {
        cout << "audio MYDecode open failed!" << endl;
        re = false;
    }

    cout << "MYAudioThread::Open :" << re << endl;
    return re;
}

void MYVideoThread::change_pause(bool is_pause)
{
    vmux.lock();
    this->is_pause = is_pause;
    vmux.unlock();
}

void MYVideoThread::run()
{
    while (!is_exit)
    {
        vmux.lock();

        if (this->is_pause)
        {
            vmux.unlock();
            msleep(5);
            continue;
        }

        // synchronize video and audio 
        if (synpts >0 && synpts < decode->pts)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }
        AVPacket *pkt = Pop();

        bool re = decode->Send(pkt);
        if (!re)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }
        // send once, receive multiple times 
        while (!is_exit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;
            //show video
            if (call)
            {
                call->Repaint(frame);
            }

        }
        vmux.unlock();
    }
}

bool MYVideoThread::pts_repaint(AVPacket *pkt, long long seekpts)
{
    vmux.lock();
    bool re = decode->Send(pkt);
    if (!re)
    {
        vmux.unlock();
        return true;   // stop
    }
    AVFrame *frame = decode->Recv();
    if (!frame)
    {
        vmux.unlock();
        return false;
    }
    if (decode->pts >= seekpts)
    {
        if (call)
            call->Repaint(frame);
        vmux.unlock();
        return true;
    }
    MYFreeFrame(&frame);
    vmux.unlock();
    return false;
}

MYVideoThread::MYVideoThread()
{
}


MYVideoThread::~MYVideoThread()
{

}
