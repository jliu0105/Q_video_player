#include <QFileInfo>
#include <QDebug>
#include <QList>
#include "MYSubTitle.h"
//#include "MYSrtParser.h"


MYSubTitle::MYSubTitle()
{

}
typedef struct subNode{
    int num;
    int beginTime;
    int endTime;
    QString text;
}subNode;
QList<subNode> subNodelist;

// open
void MYSubTitle::Open(QString url)
{
    m_mSubTitleFile.reset(new QFile(url));
    QFileInfo fileInfo(url);
    if(fileInfo.suffix() == "srt")
        m_mSubType = SUBTITLE_TYPE_SRT;
    if(m_mSubTitleFile->open(QIODevice::ReadOnly))  //set to read only
    {
        qDebug()<<"open subtitle success";
        subNodelist.clear();
        m_mIsOpen = true;
        while(!m_mSubTitleFile->atEnd())
        {
            subNode sn;
            QString line = "";
            QString cache = "";

            line = (QString)m_mSubTitleFile->readLine();               

            sn.num = line.left(line.length() - 2).toInt();               //-4 represents remove

            line = (QString)m_mSubTitleFile->readLine();               // timeline 

            int begin_hour = line.mid(0,2).toInt();
            int begin_minute = line.mid(3,2).toInt();
            int begin_second = line.mid(6,2).toInt();
            int begin_milli = line.mid(9,3).toInt();
            int beginTime = (begin_hour * 3600 + begin_minute * 60 + begin_second) * 1000 + begin_milli;  //start time
            int end_hour = line.mid(17,2).toInt();
            int end_minute = line.mid(20,2).toInt();
            int end_second = line.mid(23,2).toInt();
            int end_milli = line.mid(26,3).toInt();
            int endTime = (end_hour * 3600 + end_minute * 60 + end_second) * 1000 + end_milli;            //end time 

            sn.beginTime = beginTime;
            sn.endTime = endTime;

            line = (QString)m_mSubTitleFile->readLine();              
            while(line != "\r\n")
            {

                // return list when not found (-1)
                while(!(line.indexOf("{") == -1))                       
                    line.replace(line.indexOf("{"),line.indexOf("}") - line.indexOf("{") + 1,"");
                cache += line.left(line.length() - 2) + "\n";       
                line = (QString)m_mSubTitleFile->readLine();          
            }
            cache = cache.left(cache.length() - 1);                  

            sn.text = cache;

            subNodelist.append(sn);
        }

        return;
    }
    qDebug()<<"open subtitle file failed!";
}

bool MYSubTitle::Read(int currentTime)
{
    if(!m_mSubTitleFile->is_open())
        return false;
    for(int i = 0;i < subNodelist.count();i++)
    {
        if(subNodelist[i].beginTime < currentTime && subNodelist[i].endTime > currentTime)
        {
            m_mNodeStartTime = subNodelist[i].beginTime;
            m_mNodeEndTime = subNodelist[i].endTime;
            m_mText = subNodelist[i].text;
            return true;
        }
        if(subNodelist[i].beginTime > currentTime)
        {
            //m_mText = "";
            return false;
        }
    }
    return true;
}

bool MYSubTitle::is_open()
{
    return m_mIsOpen;
}
int MYSubTitle::start_time_node()
{
    return m_mNodeStartTime;
}
int MYSubTitle::stop_time_node()
{
    return m_mNodeEndTime;
}
QString MYSubTitle::text()
{
    return m_mText;
}
