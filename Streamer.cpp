#include "Streamer.h"

int Streamer::index = 0;
int Streamer::created=0;
int Streamer::deleted=0;
Streamer::Streamer(QString URL, enum Runner::StreamType type,QObject *parent) : QObject(parent)
{
    //qDebug()<<"Streamer::Streamer "<<URL<<" "<<type;
    created++;
    m_index=index++;

    data=NULL;

    isOver=false;
    runner = QSharedPointer<Runner>::create(m_index,&data,&h,&w,URL,type);
    thread = QSharedPointer<QThread>::create();


    this->URL=URL;

        runner->URL=URL;
        connect(thread.data(),&QThread::started,runner.data(),&Runner::run);
        connect(runner.data(), &Runner::finished,  this, &Streamer::m_quit);

        connect(runner.data(),SIGNAL(new_frame(QString)),this,SLOT(receiveFrame(QString)));
        connect(runner.data(),SIGNAL(lost_connection(QString)),this,SLOT(lostConnection(QString)));



        runner->moveToThread(thread.data());
        thread->start();


}

Streamer::~Streamer()
{

//qDebug()<<"-->Streamer::~Streamer() "<<mm->runner->thread()->isFinished()<<" "<<mm->runner->thread()->isRunning()<<" "<<m_index;

  //qDebug()<<"<--Streamer::~Streamer() "<<mm->runner->thread()->isFinished()<<" "<<mm->runner->thread()->isRunning()<<" "<<m_index;

    deleted++;
    //qDebug()<<"создано: "<<created<<" удалено: "<<deleted<<" живут: "<<created-deleted;
}

int Streamer::getFollowers() const
{
 //qDebug()<<"runner "<<runner->get_m_index()<<" get followers: "<<followers;
    return followers;
}

const QString &Streamer::getURL() const
{
    return URL;
}

bool Streamer::getIsOver() const
{
    return isOver;
}

int Streamer::get_m_index() const
{
    return m_index;
}

void Streamer::followers_inc()
{
    followers++;
    //qDebug()<<"runner "<<runner->get_m_index()<<"inc: followers "<<followers;
    frash_follower_time = QDateTime::currentDateTime();
}

void Streamer::followers_dec()
{

    int prev=followers;
    if(followers>0){
        followers--;
    }
     //qDebug()<<"runner "<<runner->get_m_index()<<" dec: followers "<<followers;
/*
    if(mm->runner->getVideoHeight()<=480&&
      mm->runner->getVideoWidth()<=640){
      //qDebug()<<"save low quality: "<<URL;
        return;
    }
    */



   //qDebug()<<"fStreamer::followers_dec. followers "<<followers<<" "<<URL<<" mode "<<mode<<" save "<<save;

    if(followers==0&&prev>0){

        no_followers=QDateTime::currentDateTime();
    }


}

bool Streamer::getSave() const
{
    return save;
}

void Streamer::setSave(bool newSave)
{
   //qDebug()<<"treamer::setSave "<<newSave<<" "<<URL;
    save = newSave;
    if(newSave==true){
    no_followers=QDateTime::currentDateTime();
    }
}

int Streamer::getW() const
{
    return w;
}

int Streamer::getH() const
{
    return h;
}



void Streamer::start()
{
//qDebug()<<"Streamer::start()";

}



void Streamer::thread_is_over()
{
 //qDebug()<<"thread_is_over for "<<m_index<<" "<<URL;
    emit signal_thread_is_over();

}

void Streamer::stop()
{
   //qDebug()<<"Streamer::stop() runner "<<runner->get_m_index();
    runner->go_to_free_state=true;
    countlost=0;
}

void Streamer::to_low_mode()
{
     runner->go_to_low_mode=true;
}

AVPicture *Streamer::getData() const
{
    return data;
}



void Streamer::receiveFrame(QString URL)
{
    //qDebug()<<"+";
    got_frame=true;
    count = 0;
    emit frame(URL);
}

void Streamer::lostConnection(QString URL)
{

    //qDebug()<<"runner "<<runner->get_m_index()<<"сигнал о потере связи "<<URL<<" followers: "<<followers;

    if(followers==0){
    stop();
    }
    emit lost(URL);

}

void Streamer::m_quit()
{
    thread->quit();
    isOver=true;

}




