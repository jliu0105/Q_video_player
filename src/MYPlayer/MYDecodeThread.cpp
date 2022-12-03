#include "MYDecode.h"
#include "MYDecodeThread.h"

void MYDecodeThread::Close()
{
    Clear();

    // wait for thread to end
    is_exit = true;
    wait();
    decode->Close();

    mux.lock();
    delete decode;
    decode = NULL;
    mux.unlock();
}

void MYDecodeThread::Clear()
{
    mux.lock();
    decode->Clear();
    while (!packs.empty())
    {
        AVPacket *pkt = packs.front();
        MYFreePacket(&pkt);
        packs.pop_front();
    }
    mux.unlock();
}


AVPacket *MYDecodeThread::Pop()
{
    mux.lock();
    if (packs.empty())
    {
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}
void MYDecodeThread::Push(AVPacket *pkt)
{
    if (!pkt)return;
    while (!is_exit)
    {
        mux.lock();
        if (packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}


MYDecodeThread::MYDecodeThread()
{
    if (!decode) decode = new MYDecode();
}


MYDecodeThread::~MYDecodeThread()
{	
    is_exit = true;
    wait();
}
