#ifndef MYAUDIOPLAY_H
#define MYAUDIOPLAY_H

class MYAudioPlay
{
public:
    int sample_ratio = 44000;
    int sample_size = 16;
    int channels = 2;

    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;
    virtual long long receive_zero_play_ms() = 0;
    // play video
    virtual bool Write(const unsigned char *data, int data_size) = 0;
    virtual int receive_free_byte() = 0;

    // pause video 
    virtual void change_pause(bool is_pause) = 0;

    // change volume
    virtual void change_volume(double new_volume) = 0;

    static MYAudioPlay *Get();
    MYAudioPlay();
    virtual ~MYAudioPlay();
};

#endif // MYAUDIOPLAY_H
