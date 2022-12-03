#include "MYVideoOutput.h"
#include <QDebug>
#include <QPainter>
#include <QTimer>
extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libavformat/avformat.h>
}

MYVideoOutput::MYVideoOutput(void)
             : m_oVideoFrame()
{
    QObject::connect(this, &MYVideoOutput::request_update,
                     this, &MYVideoOutput::procUpdate);
}

MYVideoOutput::~MYVideoOutput()
{
    sws_freeContext(m_pSwsContext);
    m_pSwsContext = nullptr;
}

void MYVideoOutput::Repaint(AVFrame *frame)
{
    if (!frame)
        {return;}

    // Convert YUV420P -> RGB32 by libswscale
    if ((nullptr == m_pSwsContext)
        || (m_oVideoFrame.width() != frame->width)
        || (m_oVideoFrame.height() != frame->height))
    {
        sws_freeContext(m_pSwsContext);
        m_pSwsContext = nullptr;
        m_pSwsContext = sws_getContext(frame->width,
                                       frame->height,
                                       static_cast<AVPixelFormat>(frame->format),
                                       frame->width,
                                       frame->height,
                                       AV_PIX_FMT_BGRA,
                                       SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }
//    int linesize[8] = {frame->linesize[0] * 3};
    int linesize[8] = {frame->width * 4};
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_BGRA, frame->width, frame->height, 1);
    uint8_t* pRGBBuffer = new uint8_t[num_bytes];
    uint8_t* szRGBBuffer[8] = {pRGBBuffer};
    sws_scale(m_pSwsContext,
              frame->data,
              frame->linesize,
              0,
              frame->height,
              szRGBBuffer,
              linesize);

    mux.lock();
    m_oVideoFrame = QImage(pRGBBuffer,
                           frame->width,
                           frame->height,
                           QImage::QImage::Format_ARGB32,
                           [](void *info)->void
                                {
                                    uint8_t* p = static_cast<uint8_t*>(info);
                                    delete[] p;
                                });
    mux.unlock();
    av_frame_free(&frame);

    //refresh
    emit request_update();
}
void MYVideoOutput::paint(QPainter *pPainter)
{


    // Draw background
    pPainter->fillRect(0, 0, width(), height(), Qt::black);

    // Draw video frame
    mux.lock();
    if (!m_oVideoFrame.isNull())
    {
        QRectF target(0, 0, width(), height());
        QRectF source(m_oVideoFrame.rect());
        pPainter->drawImage(target, m_oVideoFrame, source);
    }
    mux.unlock();
}
void MYVideoOutput::Init(int width, int height)
{
    mux.lock();
    mux.unlock();
}

void MYVideoOutput::procUpdate()
{
    update();
}
