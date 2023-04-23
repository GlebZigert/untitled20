#ifndef RUNNER_H
#define RUNNER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <time.h>

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    // Qt 4
    #include <QDeclarativeEngine>
#else
    // Qt 5
    #include <QQmlEngine>
#endif

#include <QThread>

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libswscale/swscale.h>
    #include <libavutil/hwcontext.h>
}

#include <PoolAllocator.h>

class Statistic : public QObject{

    Q_OBJECT

       public:


     explicit Statistic(QString name="");

private:

    QMap<QString,int> map;
    QString name;




   public:

    void add_to_statistics(QString addr);
    void show_statistic();
    int added;
    int deleted;

};


class Runner : public QObject
{
    Q_OBJECT


private:

public:
    QDateTime time;
    bool frash_stream;
    bool go_to_free_state;
    bool go_to_low_mode=false;
    bool return_from_low_mode = false;
    static int created;
    static int deleted;
    static int index;
    static int av_codec_open;
    static int av_codec_not_open;
    static int av_codec_close;

    static QSharedPointer<PoolAllocator<AVPicture>> allocator;

    bool losted=false;


    enum StreamType
          {
         Nothing  ,
          Storage,
             Streaming,
             Snapshot

          };
     Q_ENUMS(StreamType)

    enum Mode
           {
        TurnOff,
        Started,
        Free,
        Prepare,
        Play,
        Hold,
        Lost,
        Exit,
        Waiting,
        Low//wait first frame


           };



           Q_ENUMS(Mode)
    StreamType streamType;
    explicit Runner( QObject *parent = nullptr);
        explicit Runner(int index, QObject *parent = nullptr);
        explicit Runner(int index,AVPicture** data,int *h, int *w, QString URL,Runner::StreamType type, QObject *parent = nullptr);
    ~Runner();
    void output();

    int get_m_index() const;
    QString get_state();
    static void declareQML() {
       qmlRegisterType<Runner>("MyQMLEnums", 13, 37, "Mode");
         qmlRegisterType<Runner>("MyQMLEnums", 13, 37, "StreamType");
       qRegisterMetaType<Runner::Mode>("const Runner::Mode");


        qRegisterMetaType<Runner::StreamType>("const Runner::StreamType");
    }


    int running() const;
    int count;  // Счётчик, по которому будем ориентироваться на то,

    bool is_over;

    QString str;

    clock_t delay;
    QTimer tmr;
    static int interrupt_cb(void *ctx);
    QString URL;
    clock_t prev;
    AVPicture **data;
    int *h;
    int *w;


    int getVideoWidth() const;

    int getVideoHeight() const;

    Runner::Mode get_m_running();
    void set_m_running(Runner::Mode mode);
    void show_statistics();
    bool check_frame();
private:
    static int stream_count;
    bool first_frame_getted=false;






    static QSharedPointer<Statistic> m_pAVFrame;
    static QSharedPointer<Statistic> m_pAVCodecContext;
    static QSharedPointer<Statistic> m_pSwsContext;
    static QSharedPointer<Statistic> m_pAVPicture;
    static QSharedPointer<Statistic> m_pAVCodec;
    static QSharedPointer<Statistic> m_pFormatCtx;
    static QSharedPointer<Statistic> m_options;
    static QSharedPointer<Statistic> m_param;


    Runner::Mode m_running;
    int m_index=-1;
    AVFrame *pAVFrame, *svFrame;        // +
    AVCodecContext *pAVCodecContext;    // -

    SwsContext * pSwsContext;
    AVPicture*  pAVPicture;
    AVCodec *pAVCodec;
    AVPacket packet;
    AVFormatContext *pFormatCtx;        // ?
    AVDictionary* options;              // -
    AVCodecParameters* param ;

    unsigned int videoindex;

    int videoWidth;
    int videoHeight;

    int m_i_frameFinished;
    int ret;





void load();
bool load_settings();
void free_settings();
void free();
bool capture();

    void close();


signals:
    void finished();    // Сигнал, по которому будем завершать поток, после завершения метода run
    void lost_connection(QString URL);
    void new_frame(QString);
    void playing();
    void destroyed();



public slots:
   void run(); // Метод с полезной нагрузкой, который может выполняться в цикле               // что потоки выполняются и работают



};

#endif // RUNNER_H
