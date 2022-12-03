#include "MYPlay.h"
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <QPointer>
#include "MYDemuxThread.h"
#include "MYVideoOutput.h"
#include "MYSubTitle.h"

static QScopedPointer<MYDemuxThread> dt;
static QScopedPointer<MYSubTitle> st;

MYPlay::MYPlay()
{
    dt.reset(new MYDemuxThread);
    st.reset(new MYSubTitle);
    dt->Start();
    startTimer(40);
    setBackGround(NULL);
    setVideoPath(NULL);
    setSubTitlePath(NULL);
}

MYPlay::~MYPlay()
{
    qDebug()<<"close";
    if(m_mOpenSuccess)
    {
        QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
        qDebug()<<m_strTheVideoPath;
        settingIni->setValue("Progress/" + m_strTheVideoPath,dt->pts);
        delete settingIni;
    }
    dt->Close();
}

void MYPlay::testFunc()
{
    setDemoNum(demoNum() + 1);
}

void MYPlay::urlPass(QString url)
{
    MYVideoOutput* video =qobject_cast<MYVideoOutput*>(source());
    qDebug() << (video == nullptr) <<endl;
    m_strTheVideoPath = url;
    if(video != nullptr)
    {
        QFileInfo fileInfo(url);
        QString subTitleP = url.left(url.length() - fileInfo.suffix().length());
        QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
        QString saveProgress = settingIni->value("Progress/" + url).toString();
        qDebug()<<saveProgress;
        delete settingIni;
        // find the file for subtitle, open it there's a file
        fileInfo = QFileInfo(subTitleP+"srt");     
        if(fileInfo.exists())
        {
            st->Open(fileInfo.absoluteFilePath());
        }
        else
            qDebug()<<"there is no srt file";
        if(!dt->Open(url.toLocal8Bit(),video))
            return;
        if(saveProgress != "")
            dt->Seek(saveProgress.toDouble() / (double)dt->totalMs);
    }
    qDebug() << url;
    setIsPlay(!dt->is_pause);
    setOpenSuccess(dt->getOpenSuccess());
}

void MYPlay::posFind(double value)
{
    double pos = ((double)(int)(((double)dt->pts / (double)dt->totalMs) * 1000)) / 1000;
    double newValue = ((double)(int)(value * 1000)) / 1000;
    qDebug() << "pos value = " << pos;
    dt->Seek(newValue);
}

void MYPlay::timerEvent(QTimerEvent *e)
{
    setTestNum(m_mTestNum + 1);
    long long total = dt->totalMs;
    int currentTime = (int)dt->pts;
    if(st->is_open())
    {
        if(!(st->start_time_node() <= currentTime && st->stop_time_node() >= currentTime) || (st->stop_time_node() == st->start_time_node()))   //如果已经读取到现在的节点则没必要在read，或者处于刚开始时需要执行read
            if(st->Read(currentTime))
                setSubTitleText(st->text());
            else
                setSubTitleText("");
    }
    if (total > 0)
    {
        double pos = ((double)(int)(((double)dt->pts / (double)total) * 1000)) / 1000;     //保留三位小数，后面置0，方便比较
        setPosNum(pos);
    }
}
void MYPlay::playOrPause()
{
    bool is_pause = !dt->is_pause;
    setIsPlay(!is_pause);
    dt->change_pause(is_pause);
}
void MYPlay::goOn()
{
    double pos = ((double)(int)(((double)dt->pts / (double)dt->totalMs) * 1000)) / 1000 + 0.02;   //依照需求调整快进的程度
    dt->Seek(pos);
}
void MYPlay::goBack()
{
    double pos = ((double)(int)(((double)dt->pts / (double)dt->totalMs) * 1000)) / 1000 - 0.02;   //依照需求调整快退的程度
    dt->Seek(pos);
}
void MYPlay::changeBackground(QString url)
{
    setBackGround(url);
}
void MYPlay::changeVideoPath(QString url)
{
    setVideoPath(url);
}
void MYPlay::changeSubTitlePath(QString url)
{
    setSubTitlePath(url);
}
void MYPlay::change_volume(double new_volume)
{
    dt->change_volume(((double)(int)(new_volume*100))/100);
}
void MYPlay::setStop()
{
    if(m_mOpenSuccess)
    {
        dt->Close();
        dt.reset(new MYDemuxThread);
        dt->Start();
        if(st->is_open())
        {
            st.reset(new MYSubTitle);
            setSubTitleText("");
        }
        setOpenSuccess(false);
    }
}
void MYPlay::loadSubTitle(QString url)
{
    if(m_mOpenSuccess)
    {
        st.reset(new MYSubTitle);
        st->Open(url);
    }
}
int MYPlay::demoNum() const
{
    return m_mDemoNum;
}

void MYPlay::setDemoNum(int newValue)
{
    if (m_mDemoNum != newValue)
    {
        m_mDemoNum = newValue;
        emit demoNumChanged(m_mDemoNum);
    }
}

int MYPlay::testNum() const
{
    return m_mTestNum;
}

void MYPlay::setTestNum(int newValue)
{
    if (m_mTestNum != newValue)
    {
        m_mTestNum = newValue;
        emit testNumChanged(m_mTestNum);
    }
}

double MYPlay::posNum() const
{
    return m_mPosNum;
}

void MYPlay::setPosNum(double newValue)
{
    if (m_mPosNum != newValue)
    {
        m_mPosNum = newValue;
        emit posNumChanged(m_mPosNum);
    }
}

bool MYPlay::isPlay() const
{
    return m_mIsPlaying;
}

void MYPlay::setIsPlay(bool isPlaying)
{
    if(m_mIsPlaying != isPlaying)
    {
        m_mIsPlaying = isPlaying;
        emit isPlayingChanged(m_mIsPlaying);
    }
}

bool MYPlay::openSuccess() const
{
    return m_mOpenSuccess;
}

void MYPlay::setOpenSuccess(bool openSuccess)
{
    if(m_mOpenSuccess != openSuccess)
    {
        m_mOpenSuccess = openSuccess;
        emit openSuccessChanged(m_mOpenSuccess);
    }
}

QObject* MYPlay::source() const
{
    return m_mSource;

}
void MYPlay::setSource(QObject *source)
{
    if(m_mSource != source)
    {
        m_mSource = source;
        emit sourceChanged(m_mSource);
    }
}
QString MYPlay::backGround() const
{
    return m_strBackgroundPath;
}
void MYPlay::setBackGround(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString backGroundpath = settingIni->value("Path/Background").toString();
    if(url == NULL)
    {
        if(backGroundpath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strBackgroundPath = backGroundpath;
            QFileInfo fileInfo(backGroundpath);
            m_strBackGroundChoosePath = fileInfo.path();
            qDebug()<<m_strBackGroundChoosePath<<endl;
            delete settingIni;
            emit backGroundChanged(m_strBackgroundPath);
            emit backGroundChooseChanged(m_strBackGroundChoosePath);
            return;
        }
    }
    if(m_strBackgroundPath != url)
    {
        settingIni->setValue("Path/Background",url);
        m_strBackgroundPath = url;
        QFileInfo fileInfo(url);
        //qDebug()<<fileInfo.suffix();
        m_strBackGroundChoosePath = fileInfo.path();
        emit backGroundChanged(m_strBackgroundPath);
        emit backGroundChooseChanged(m_strBackGroundChoosePath);
    }
    delete settingIni;
}
QString MYPlay::backGroundChoose() const
{
    return m_strBackGroundChoosePath;
}

QString MYPlay::videoPath() const
{
    return m_strVideoPath;
}
void MYPlay::setVideoPath(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString videoPath = settingIni->value("Path/VideoChoose").toString();
    if(url == NULL)
    {
        if(videoPath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strVideoPath = videoPath;
            delete settingIni;
            emit videoPathChanged(m_strVideoPath);
            return;
        }
    }
    if(m_strVideoPath != url)
    {
        QFileInfo fileInfo(url);
        m_strVideoPath = fileInfo.path();
        settingIni->setValue("Path/VideoChoose",m_strVideoPath);
        emit videoPathChanged(m_strVideoPath);
    }
    delete settingIni;
}
QString MYPlay::subTitlePath() const
{
    return m_strSubTitlePath;
}
void MYPlay::setSubTitlePath(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString subTitlePath = settingIni->value("Path/SubTitleChoose").toString();
    if(url == NULL)
    {
        if(subTitlePath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strSubTitlePath = subTitlePath;
            delete settingIni;
            emit subTitlePathChanged(m_strSubTitlePath);
            return;
        }
    }
    if(m_strSubTitlePath != url)
    {
        QFileInfo fileInfo(url);
        m_strSubTitlePath = fileInfo.path();
        settingIni->setValue("Path/SubTitleChoose",m_strSubTitlePath);
        emit subTitlePathChanged(m_strSubTitlePath);
    }
    delete settingIni;
}

QString MYPlay::subTitleText() const
{
    return m_mSubTitleText;
}
void MYPlay::setSubTitleText(QString text)
{
    if(m_mSubTitleText != text)
    {
        m_mSubTitleText = text;
        emit subTitleTextChanged(m_mSubTitleText);
    }
}
