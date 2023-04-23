#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QtQuick/QQuickPaintedItem>
#include <QImage>
#include <QThread>
#include <QTimer>
#include "Streamer.h"
#include "streamercontaineraccesser.h"

class VideoPlayer : public QQuickPaintedItem

{
    Q_OBJECT

    Q_PROPERTY(bool connection READ connection  NOTIFY connectionChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int cid READ getCid WRITE setCid NOTIFY cidChanged)
public:
    explicit  VideoPlayer(QQuickItem *parent = 0);
    ~VideoPlayer();
    void paint(QPainter *painter) override;



    bool connection() const;

    QString source() const;
    void setSource(const QString source);


    QTimer cleaner;

    QTimer timer;

    QTimer wait_for_next;


    Q_INVOKABLE void start(Runner::StreamType type);
    Q_INVOKABLE void  stop();
    Q_INVOKABLE void  shot();

        Q_INVOKABLE void  info();

    Q_INVOKABLE void  saving_on();
    Q_INVOKABLE void  saving_off();

    Q_INVOKABLE void  clear();

    Q_INVOKABLE void  delete_free_streamers();

    Runner::StreamType streamType;

    AVPicture *data;
    int h;
    int w;

    int cid;

    bool m_connection;


    Q_INVOKABLE Runner::StreamType getMode();

    Q_INVOKABLE int getCid() const;
    void setCid(int newCid);

private:

    QString     m_source;

    QImage img;

    QSharedPointer<StreamerContainer> container;

    QSharedPointer<Streamer> current = nullptr;
    QSharedPointer<Streamer> next = nullptr;

signals:
    void playing();
    void sourceChanged(const QString &source);
    void connectionChanged(const bool &connection);
    void cidChanged(const int &cid);
public slots:

    void onWidthChanged();
    void onheightChanged();
    void frame(QString src);

    void f_clear();
    void on_timer();
    void f_wait_for_next();
    void next_frame(QString src);

};

#endif // VIDEOPLAYER_H
