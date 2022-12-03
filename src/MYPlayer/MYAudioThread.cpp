#include <iostream>
#include "MYDecode.h"
#include "MYAudioPlay.h"
#include "MYResample.h"
#include "MYAudioThread.h"

using namespace std;


void MYAudioThread::Clear()
{
    MYDecodeThread::Clear();
    mux.lock();
    if (ap)ap->Clear();
    mux.unlock();
}

// stop thread, clean up resource
void MYAudioThread::Close()
{
    MYDecodeThread::Close();
    if (res)
    {
        res->Close();
        amux.lock();
        delete res;
        res = NULL;
        amux.unlock();
    }
    if (ap)
    {
        ap->Close();
        amux.lock();
        ap = NULL;
        amux.unlock();
    }
}

bool MYAudioThread::Open(AVCodecParameters *para, int sample_ratio, int channels)
{
    if (!para)return false;
    Clear();

    amux.lock();
    pts = 0;
    bool re = true;
    if (!res->Open(para, false))
    {
        cout << "MYResample open failed!" << endl;
        re = false;
    }
    ap->sample_ratio = sample_ratio;
    ap->channels = channels;
    if (!ap->Open())
    {
        re = false;
        cout << "MYAudioPlay open failed!" << endl;
    }
    if (!decode->Open(para))
    {
        cout << "Audio MYDecode open failed!" << endl;
        re = false;
    }
    amux.unlock();
    cout << "MYAudioThread::Open success! : " << re << endl;
    return re;
}

void MYAudioThread::change_pause(bool is_pause)
{
    this->is_pause = is_pause;
    if (ap)
        ap->change_pause(is_pause);
}

void MYAudioThread::change_volume(double new_volume)
{
    if(!ap)
    {
        return;
    }
    ap->change_volume(new_volume);
}

void MYAudioThread::run()
{
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!is_exit)
    {
        amux.lock();
        if (is_pause)
        {
            amux.unlock();
            msleep(6);
            continue;
        }

        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            amux.unlock();
            msleep(2);
            continue;
        }
        while (!is_exit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;

            pts = decode->pts - ap->receive_zero_play_ms();

            int size = res->sample_again(frame, pcm);
            // play video 
            while (!is_exit)
            {
                if (size <= 0)break;
                // not enough space 
                if (ap->receive_free_byte() < size || is_pause)
                {
                    msleep(2);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        amux.unlock();
    }
    delete[] pcm;
}

MYAudioThread::MYAudioThread()
{
    if (!res) res = new MYResample();
    if (!ap) ap = MYAudioPlay::Get();
}


MYAudioThread::~MYAudioThread()
{
    // wait for thread to exit 
    is_exit = true;
    wait();
}
