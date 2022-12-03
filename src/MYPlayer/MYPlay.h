#ifndef MYPLAY_H
#define MYPLAY_H

#include <QObject>
class MYPlay : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int demoNum READ demoNum WRITE setDemoNum NOTIFY demoNumChanged)

    Q_PROPERTY(int testNum READ testNum WRITE setTestNum NOTIFY testNumChanged)

    Q_PROPERTY(double posNum READ posNum WRITE setPosNum NOTIFY posNumChanged)

    Q_PROPERTY(QObject* source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool isPlay READ isPlay WRITE setIsPlay NOTIFY isPlayingChanged)

    Q_PROPERTY(bool openSuccess READ openSuccess WRITE setOpenSuccess NOTIFY openSuccessChanged)

    Q_PROPERTY(QString backGround READ backGround WRITE setBackGround NOTIFY backGroundChanged)

    Q_PROPERTY(QString backGroundChoose READ backGroundChoose WRITE setBackGround NOTIFY backGroundChooseChanged)

    Q_PROPERTY(QString videoPath READ videoPath WRITE setVideoPath NOTIFY videoPathChanged)

    Q_PROPERTY(QString subTitlePath READ subTitlePath WRITE setSubTitlePath NOTIFY subTitlePathChanged)

    Q_PROPERTY(QString subTitleText READ subTitleText WRITE setSubTitleText NOTIFY subTitleTextChanged)
public:
    MYPlay();
    ~MYPlay();
public:
    Q_INVOKABLE void testFunc();

    Q_INVOKABLE void urlPass(QString url);
    Q_INVOKABLE void posFind(double value);
    Q_INVOKABLE void playOrPause();
    Q_INVOKABLE void goOn();
    Q_INVOKABLE void goBack();
    Q_INVOKABLE void changeBackground(QString url);
    Q_INVOKABLE void changeVideoPath(QString url);
    Q_INVOKABLE void changeSubTitlePath(QString url);
    Q_INVOKABLE void change_volume(double new_volume);
    Q_INVOKABLE void setStop();
    Q_INVOKABLE void loadSubTitle(QString url);

    void timerEvent(QTimerEvent *e);

    QObject *source() const;

    void setSource(QObject *source);

    int demoNum() const;
    void setDemoNum(int newValue);

    int testNum() const;
    void setTestNum(int newValue);

    double posNum() const;
    void setPosNum(double newValue);

    bool isPlay() const;
    void setIsPlay(bool isPlaying);

    bool openSuccess() const;
    void setOpenSuccess(bool openSuccess);
    QString backGround() const;
    void setBackGround(QString url);
    QString backGroundChoose() const;
    QString videoPath() const;
    void setVideoPath(QString url);
    QString subTitlePath() const;
    void setSubTitlePath(QString url);
    QString subTitleText() const;
    void setSubTitleText(QString text);

signals:
    void demoNumChanged(int newValue);
    void testNumChanged(int newValue);
    void posNumChanged(double newValue);
    void sourceChanged(QObject *object);
    void isPlayingChanged(bool isPlaying);
    void openSuccessChanged(bool openSuccess);
    void backGroundChanged(QString url);
    void backGroundChooseChanged(QString url);
    void videoPathChanged(QString url);
    void subTitlePathChanged(QString url);
    void subTitleTextChanged(QString text);

private:
    int                                         m_mDemoNum{ 0 };

    int                                         m_mTestNum{ 0 };

    double                                      m_mPosNum{ 0.0 };

    QObject*                                    m_mSource{nullptr};

    bool                                        m_mIsPlaying{false};

    bool                                        m_mOpenSuccess{false};

    QString                                     m_strBackgroundPath{""};

    QString                                     m_strBackGroundChoosePath{""};
    
    QString                                     m_strVideoPath{""};

    QString                                     m_strTheVideoPath{ "" };

    QString                                     m_strSubTitlePath{""};

    QString                                     m_mSubTitleText{""};
};

#endif // MYPLAY_H
