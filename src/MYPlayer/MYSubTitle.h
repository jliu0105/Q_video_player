#ifndef MYSUBTITLE_H
#define MYSUBTITLE_H

#include <QObject>
#include <QPointer>
#include <QFile>

class MYSubTitle : public QObject
{
    Q_OBJECT
public:
    MYSubTitle();

    void Open(QString url);
    bool Read(int currentTime);

    bool is_open();
    int start_time_node();
    int stop_time_node();
    QString text();

    enum SUBTITLE_TYPE{
        SUBTITLE_TYPE_UNKNOW = 0,
        SUBTITLE_TYPE_SRT,
        SUBTITLE_TYPPE_SSA      
        //...
    };
    Q_ENUM(SUBTITLE_TYPE)   

private:

    SUBTITLE_TYPE                                              m_mSubType{ SUBTITLE_TYPE_UNKNOW };

    QScopedPointer<QFile>                                      m_mSubTitleFile;

    int                                                        m_mNodeStartTime{ 0 };

    int                                                        m_mNodeEndTime{ 0 };

    QString                                                    m_mText{ "" };

    bool                                                       m_mIsOpen{ false };                    
};

#endif 
