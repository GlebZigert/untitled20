#ifndef STREAMERCONTAINER_H
#define STREAMERCONTAINER_H

#include <QObject>
#include <QMap>
#include "Streamer.h"
#include <QSharedPointer>
#include <QTimer>

class StreamerContainer : public QObject
{
    Q_OBJECT

private:



public:
     void func();

     void show();

    QTimer timer;

    explicit StreamerContainer(QObject *parent = nullptr);

     QList<QSharedPointer<Streamer>> map;

     QSharedPointer<Streamer> start(QString url, Runner::StreamType type);

     void delete_free_streamers();

     bool flag=false;

     QSharedPointer<Streamer> find(QString url,Runner::StreamType type);

public slots:
     void thread_is_over();
    void on_timer();

signals:

};

#endif // STREAMERCONTAINER_H
