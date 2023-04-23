#include "videoplayer.h"
#include <QPainter>



VideoPlayer::VideoPlayer(QQuickItem *parent):QQuickPaintedItem(parent)
{
  //  //qDebug()<<"VideoPlayer::VideoPlayer";
    container = StreamerContainerAccesser::get();

    m_connection = true;

    data=NULL;

    connect(&timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    connect(&cleaner, SIGNAL(timeout()), this, SLOT(f_clear()));
    connect(&wait_for_next, SIGNAL(timeout()), this, SLOT(f_wait_for_next()));

  //  list1=new Streamer(&data,&h,&w);
  //  list1->URL="rtsp://root:root@192.168.0.187:50554/hosts/ASTRAAXXON/DeviceIpnt.1/SourceEndpoint.video:0:0";

  //  connect(list1,SIGNAL(frame(QString)),this,SLOT(frame(QString)));
  //  connect(list1,SIGNAL(lost(QString)),this,SLOT(lost(QString)));

}

VideoPlayer::~VideoPlayer()
{
 //qDebug()<<"--> ~VideoPlayer::VideoPlayer";

    if(current){
        //если мы уже принимаем поток - нужно от него отписаться
        disconnect(current.data(),SIGNAL(frame(QString)),this,SLOT(frame(QString)));


        //qDebug()<<"clear "<<current.data()->getURL();

        data=NULL;
        current->followers_dec();
        current.clear();
    }
    if(next){
        next->followers_dec();
     disconnect(next.data(),SIGNAL(frame(QString)),this,SLOT(next_frame(QString)));

     next.clear();
    }


//qDebug()<<"<-- ~VideoPlayer::VideoPlayer";
}


void VideoPlayer::paint(QPainter *painter)
{
    if(current)
    if(current.data())
    {
data = current.data()->getData();
    if(data!=NULL){
 /*   QImage img=QImage(data->data[0],
            w,
            h,
            QImage::Format_RGB32);
*/

    painter->drawImage(QRect(0, 0, this->width(), this->height()), img);
 //   //qDebug()<<"+ "<<this->width()<<" "<<this->height()<<" "<<img.size();
    }
    }

}

bool VideoPlayer::connection() const
{
    return m_connection;
}

QString VideoPlayer::source() const
{
    return m_source;
}

void VideoPlayer::setSource(const QString source)
{

    m_source=source;
    emit sourceChanged(m_source);
}





void VideoPlayer::start(Runner::StreamType type)
{
   qDebug()<<"VideoPlayer::start "<<m_source<<" "<<type;
   if(next){
       next->followers_dec();
    disconnect(next.data(),SIGNAL(frame(QString)),this,SLOT(next_frame(QString)));

    next.clear();
   }
    timer.stop();
    streamType=Runner::Nothing;



    if(m_source==""){
    //    img=QImage(":/qml/video/.jpeg");
  // this->update();
        return;
    }




  next = container->start(m_source,type);

  if(next){
            //qDebug()<<"видеоплеер "<<cid<<" нашел следующий runner "<<next->runner->get_m_index();
         next->followers_inc();

          connect(next.data(),SIGNAL(frame(QString)),this,SLOT(next_frame(QString)));

  }

  wait_for_next.start(1000);

}

void VideoPlayer::stop()
{
        timer.stop();
    //qDebug()<<"VideoPlayer::stop() ";
    if(current){
        //qDebug()<<"VideoPlayer::stop() runner "<<current->runner->get_m_index();
    disconnect(current.data(),SIGNAL(frame(QString)),this,SLOT(frame(QString)));


    //qDebug()<<"clear "<<current.data()->getURL();
    current.data()->save=false;

 //   data=NULL;
    current->followers_dec();
    current.clear();

    }

    if(next){
        next->followers_dec();
     disconnect(next.data(),SIGNAL(frame(QString)),this,SLOT(next_frame(QString)));

     next.clear();
    }
    timer.stop();
    wait_for_next.stop();

    img=QImage(":/qml/video/no_in_storage.jpeg");
this->update();

     container->func();
 // list1->stop();
}

void VideoPlayer::shot()
{

    if(m_source==""){
        img=QImage(":/qml/video/no_in_storage.jpeg");
   this->update();
    }

}

void VideoPlayer::info()
{
container->show();
}

void VideoPlayer::saving_on()
{
    if(current!=0){
        current.data()->setSave(true);
    }
}

void VideoPlayer::saving_off()
{
    if(current!=0){
        current.data()->setSave(false);
    }

}

void VideoPlayer::clear()
{
        timer.stop();
    cleaner.start(1000);
}

void VideoPlayer::delete_free_streamers()
{
container->delete_free_streamers();
    }

Runner::StreamType VideoPlayer::getMode()
{
if(current){
return current->runner->streamType;
}
    return Runner::StreamType::Nothing;
}

int VideoPlayer::getCid() const
{
 //   //qDebug()<<"VideoPlayer::getCid()";
    return cid;
}

void VideoPlayer::setCid(int newCid)
{
   //qDebug()<<"VideoPlayer::setCid(): "<< newCid;
    cid = newCid;
        emit cidChanged(cid);
}



void VideoPlayer::onWidthChanged(){
    update();
}

void VideoPlayer::onheightChanged(){
   update();
}

void VideoPlayer::frame(QString source){

//    //qDebug()<<"VideoPlayer::frame "<<streamType;


    if(current.data()){
    data = current.data()->getData();
    if(data!=NULL){
    w = current.data()->getW();
    h = current.data()->getH();

    if(current->runner->check_frame())
    if(source==this->m_source&&data!=NULL){

        cleaner.stop();
         img=QImage(data->data[0],
                    w,
                    h,
                    QImage::Format_RGB32);

    m_connection = true;
    this->update();
    }
    }



    }


//    timer.stop();
    if(streamType!=Runner::StreamType::Snapshot) {
    timer.start(200);
    }

}


void VideoPlayer::f_clear()
{
    img=QImage();
    this->update();
}

void VideoPlayer::on_timer()
{
//qDebug()<<"VideoPlayer::on_timer()";
//    timer.stop();
    m_connection = false;



 //   //qDebug()<<"videoplayer lost runner"<<current->runner->get_m_index()<<" "<<current->runner->URL;
    //stop();
    img=QImage(":/qml/video/no_signal.jpeg");
     //  this->update();
    emit connectionChanged(m_connection);
}

void VideoPlayer::f_wait_for_next()
{


//qDebug()<<"VideoPlayer::f_wait_for_next()";

       //qDebug()<<"videoplayer lost runner";
       // stop();
      //  img=QImage(":/qml/video/no_signal.jpeg");
      //     this->update();
        m_connection = false;
        emit connectionChanged(m_connection);

}

void VideoPlayer::next_frame(QString src)
{
    wait_for_next.stop();

    //qDebug()<<"--> VideoPlayer::next_frame() from runner "<<next->runner->get_m_index()<<" cid "<<cid<<" src "<<m_source;
    disconnect(next.data(),SIGNAL(frame(QString)),this,SLOT(next_frame(QString)));

    if(current){

        //streamType=current->runner->streamType;
        //если мы уже принимаем поток - нужно от него отписаться
        disconnect(current.data(),SIGNAL(frame(QString)),this,SLOT(frame(QString)));


        //qDebug()<<"clear "<<current.data()->getURL();

        data=NULL;
        current->followers_dec();
        current.clear();
    }

    current=next;
    next.clear();


      data = current.data()->getData();

      connect(current.data(),SIGNAL(frame(QString)),this,SLOT(frame(QString)));

      streamType=current->runner->streamType;
    //  //qDebug()<<"streamType = "<<streamType;
    //  m_connection=true;

   //qDebug()<<"<-- VideoPlayer::next_frame() from runner ";
  timer.start(200);
}









