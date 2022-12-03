#include <mutex>
#include <QAudioFormat>
#include <QAudioOutput>
#include "MYAudioPlay.h"


class CMYAudioPlay :public MYAudioPlay
{
public:
    QAudioOutput *output = NULL;
    QIODevice *io = NULL;
    std::mutex mux;

    // modify volume
    virtual void change_volume(double new_volume)
    {
        if(!output)
        {
            return;
        }
        output->change_volume(new_volume);
    }

    virtual long long receive_zero_play_ms()
    {
        mux.lock();
        if (!output)
        {
            mux.unlock();
            return 0;
        }

        long long pts = 0;
        // byte size which has not played
        double size = output->bufferSize() - output->bytesFree();
        // size for one second 
        double size_second = sample_ratio * (sample_size / 8) *channels;

        if (size_second <= 0)
        {
            pts = 0;
        }
        else
        {
            pts = (size / size_second) * 1000;
        }

        mux.unlock();
        return pts;
    }


    void Clear()
    {
        mux.lock();
        if (io)
        {
            io->reset();
        }
        mux.unlock();
    }


    virtual void Close()
    {
        mux.lock();
        if (io)
        {
            io->close();
            io = NULL;
        }
        if (output)
        {
            output->stop();
            delete output;
            output = 0;
        }
        mux.unlock();
    }
    virtual bool Open()
    {
        Close();
        QAudioFormat fmt;
        fmt.setSampleRate(sample_ratio);
        fmt.setSampleSize(sample_size);
        fmt.setChannelCount(channels);
        fmt.setCodec("audio/pcm");
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        mux.lock();
        output = new QAudioOutput(fmt);
        io = output->start(); //start playing
        mux.unlock();
        if (io)
            return true;
        return false;
    }
    // pause video
    virtual void change_pause(bool is_pause)
    {
        mux.lock();
        if (!output)
        {
            mux.unlock();
            return;
        }
        if (is_pause)
        {
            output->suspend();
        }
        else
        {
            output->resume();
        }
        mux.unlock();
    }

    // play video 
    virtual bool Write(const unsigned char *data, int data_size)
    {
        if (!data || data_size <= 0)return false;
        mux.lock();
        if (!output || !io)
        {
            mux.unlock();
            return false;
        }
        int size = io->write((char *)data, data_size);
        mux.unlock();
        if (data_size != size)
            return false;
        return true;
    }

    virtual int receive_free_byte()
    {
        mux.lock();
        if (!output)
        {
            mux.unlock();
            return 0;
        }
        int free = output->bytesFree();
        mux.unlock();
        return free;
    }
};
MYAudioPlay *MYAudioPlay::Get()
{
    static CMYAudioPlay play;
    return &play;
}

MYAudioPlay::MYAudioPlay()
{
}


MYAudioPlay::~MYAudioPlay()
{
}
