#include "runner.h"
#include "QDebug"
#include <mutex>
int Runner::created=0;
int Runner::deleted=0;
int Runner::index=1;
int Runner::av_codec_open=0;
int Runner::av_codec_not_open=0;
int Runner::av_codec_close=0;
int Runner::stream_count=0;

QSharedPointer<Statistic> Runner::m_pAVFrame=QSharedPointer<Statistic>::create("m_pAVFrame");
QSharedPointer<Statistic> Runner::m_pAVCodecContext=QSharedPointer<Statistic>::create("m_pAVCodecContext");
QSharedPointer<Statistic> Runner::m_pSwsContext=QSharedPointer<Statistic>::create("m_pSwsContext");
QSharedPointer<Statistic> Runner::m_pAVPicture=QSharedPointer<Statistic>::create("m_pAVPicture");
QSharedPointer<Statistic> Runner::m_pAVCodec=QSharedPointer<Statistic>::create("m_pAVCodec");
QSharedPointer<Statistic> Runner::m_pFormatCtx=QSharedPointer<Statistic>::create("m_pFormatCtx");
QSharedPointer<Statistic> Runner::m_options=QSharedPointer<Statistic>::create("m_options");
QSharedPointer<Statistic> Runner::m_param=QSharedPointer<Statistic>::create("m_param");

#define AVIO_FLAG_NONBLOCK   8

static std::mutex local_mutex;

Runner::Runner( QObject *parent) : QObject(parent)
{
    //qDebug()<<"Runner::Runner( QObject *parent)";
  //  //qDebug()<<"Runner::Runner( QObject *parent) : QObject(parent)";
    av_log_set_level(AV_LOG_QUIET);
    frash_stream=false;
    go_to_free_state=false;

   allocator=QSharedPointer<TwoSlotAllocator<AVPicture>>::create(sizeof(AVPicture));


created++;
     videoHeight=1920;
     videoWidth=1080;
     pAVCodecContext = NULL;

     svFrame = NULL;
      pSwsContext = NULL;
     pAVPicture = NULL;
     pAVCodec = NULL;
     options = NULL;
     pFormatCtx = NULL;

     param  = NULL;

     pAVFrame = av_frame_alloc();
       param = avcodec_parameters_alloc();
  //  av_init_packet(&packet);
}

Runner::~Runner()
{
//qDebug()<<"Runner::~Runner()-->";
     local_mutex.lock();
  //qDebug()<<"DELETE Runner "<<m_index;

    close();

    if(pAVFrame!=NULL){
        //qDebug()<<"av_frame_free(&pAVFrame); ";
        av_frame_unref(pAVFrame);
    av_frame_free(&pAVFrame);
    }

    if(options!=NULL){
//qDebug()<<"  av_dict_free(&options);";
        av_dict_free(&options);
        options=NULL;
    }

    if(param)   {
   //qDebug()<<"  avcodec_parameters_free(&param);";
   avcodec_parameters_free(&param);
    }
  //qDebug()<<"runner destroyed "<<m_index<<" ";
    local_mutex.unlock();
    deleted++;
    //qDebug()<<"Runner::~Runner()<--";
    //qDebug()<<"раннеров  создано: "<<created<<" удалено: "
           //<<deleted<<" живут: "
          //<<created-deleted;

}

Runner::Runner(int index, QObject *parent ):Runner(parent)
{
    //qDebug()<<"Runner::Runner(int index, QObject *parent )";

    m_running = Mode::Started;


    set_m_running(Mode::Free);

    frash_stream=false;
    streamType=Runner::StreamType::Nothing;

    m_index=index;
}


Runner::Runner(int index, AVPicture **data, int *h, int *w, QString URL, Runner::StreamType type, QObject *parent)
    :Runner(index)
{
//qDebug()<<"Runner(int index, AVPicture **data, int *h, int *w, QString URL, Runner::Mode mode, QObject *parent)";
    videoHeight=1920;
    videoWidth=1080;

      //qDebug()<<"Runner::Runner( QObject *parent) : QObject(parent)";


    this->data=data;
    this->URL=URL;
    this->h=h;
    this->w=w;
    streamType=type;

    frash_stream=true;
    set_m_running(Mode::Prepare);


}




int Runner::get_m_index() const
{
    return m_index;
}

QString Runner::get_state()
{
    QString res="";
switch(m_running){
case 0:
    res="TurnOff ";
break;
case 1:
    res="Started ";
break;
case 2:
    res="Free    ";
break;
case 3:
    res="Prepare ";
break;
case 4:
    res="Play    ";
break;
case 5:
    res="Hold    ";
break;
case 6:
    res="Lost    ";
break;
case 7:
    res="Exit    ";
break;
case 8:
    res="Waiting ";
break;
case 9:
    res="Low     ";
break;
break;
}
return res;
}

int Runner::running() const
{

}

int Runner::interrupt_cb(void *ctx)
{
//qDebug()<<"interrupt_cb";
    Runner* pl=(Runner*)ctx;
    pl->delay=clock()-pl->prev;
    //  //qDebug()<<pl->delay;
    if(pl->delay>300000){
           //qDebug()<<"Interrupt runner "<<pl->m_index;
        pl->prev=clock();
        pl->set_m_running(Mode::TurnOff);
        emit pl->lost_connection(pl->URL);

        return 1;
    }
    return 0;

}

int Runner::getVideoWidth() const
{
    return videoWidth;
}

int Runner::getVideoHeight() const
{
    return videoHeight;
}

Runner::Mode Runner::get_m_running()
{
    return m_running;
}

void Runner::set_m_running(Mode mode)
{
    //qDebug()<<"Runner "<<m_index<<" ::set_m_running "<<mode;
    m_running=mode;
}

bool Runner::check_frame()
{
    return first_frame_getted;
}



void Runner::load()
{


}




void Runner::show_statistics()
{
    qDebug()<<"всего было запрошено потоков: "<<stream_count;


m_pAVFrame->show_statistic();
m_pAVCodecContext->show_statistic();
m_pSwsContext->show_statistic();
m_pAVPicture->show_statistic();
m_pAVCodec->show_statistic();
m_pFormatCtx->show_statistic();
m_options->show_statistic();
m_param->show_statistic();
    /*
    qDebug()<<"сколько  всего открывал потоков: "<<stream_count;
    show_statistic(&m_pAVFrame,"m_pAVFrame");
    show_statistic(&m_pAVCodecContext,"m_pAVCodecContext");
    show_statistic(&m_pSwsContext,"m_pSwsContext");
    show_statistic(&m_pAVPicture,"m_pAVPicture");
    show_statistic(&m_pAVCodec,"m_pAVCodec");
    show_statistic(&m_pFormatCtx,"m_pFormatCtx");
    show_statistic(&m_options,"m_options");
    show_statistic(&m_param,"m_param");
    */
    qDebug()<<" ";
}

bool Runner::load_settings()
{

    stream_count++;
    //qDebug()<<"runner "<<m_index<<" --> pAVFrame "<<QString::number((qint64)pAVFrame,16)<<" sizeof(pAVFrame): "<<sizeof(*pAVFrame);


    m_pAVFrame->add_to_statistics(QString::number((qint64)pAVFrame,16));

//   QString URL="rtsp://root:root@192.168.0.187:50554/hosts/ASTRAAXXON/DeviceIpint.1/SourceEndpoint.video:0:0";
    ////qDebug()<<"URL: "<<URL;

    QByteArray ba = URL.toLatin1();
    char *c_str2 = ba.data();
    char *filepath = ba.data();

  ////qDebug()<<"-->";
  ////qDebug()<<"<--";

 ////qDebug()<<"av_dict_set-->";
    //qDebug()<<"runner "<<m_index<<" --> options "<<options<<" sizeof(options): "<<sizeof(&options);
    av_dict_set(&options, "buffer_size", "1024000", 0); //Set the cache size, 1080p can increase the value
    av_dict_set(&options, "rtsp_transport", "tcp", 0); //Open in udp mode, if open in tcp mode, replace udp with tcp
    av_dict_set(&options, "stimeout", "200000", 0); //Set timeout disconnect time, unit is microsecond "20000000"
    av_dict_set(&options, "max_delay", "1000", 0); //Set the maximum delay
    //qDebug()<<"runner "<<m_index<<" <-- options "<<options<<" sizeof(options): "<<sizeof(&options);
   m_options->add_to_statistics(QString::number((qint64)options,16));
    ////qDebug()<<"runner "<<m_index<<" options: "<<options;
    ////qDebug()<<"avformat_open_input -->";
   ////qDebug()<<"av_dict_set<--";


    //qDebug()<<"runner "<<m_index<<" --> pFormatCtx "<<pFormatCtx<<" sizeof(pFormatCtx): "<<sizeof(*pFormatCtx);

    int error = avformat_open_input(&pFormatCtx, filepath, NULL, &options);
    //qDebug()<<"runner "<<m_index<<" <-- pFormatCtx "<<pFormatCtx<<" sizeof(pFormatCtx): "<<sizeof(*pFormatCtx);
  m_pFormatCtx->add_to_statistics(QString::number((qint64)pFormatCtx,16));
    if (error != 0){



        return false;
    }

    prev=clock();
    pFormatCtx->interrupt_callback.callback=interrupt_cb;
    pFormatCtx->interrupt_callback.opaque = this;

  ////qDebug()<<"av_init_packet-->";

  ////qDebug()<<"av_init_packet<--";

    pFormatCtx->probesize = 1000;
    pFormatCtx->max_analyze_duration = AV_TIME_BASE;

   ////qDebug()<<"avformat_find_stream_info-->";
    if (avformat_find_stream_info(pFormatCtx, NULL)<0){


        ////qDebug()<<"FAIL with: avformat_find_stream_info ";
        return false;
    }
   ////qDebug()<<"avformat_find_stream_info<--";

    videoindex = -1;
    for (int i = 0; i<pFormatCtx->nb_streams; i++)
    if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
        videoindex = i;
        break;
    }


    if (videoindex == -1){


         ////qDebug()<<"FAIL with: videoindex ";
        return false;
    }

    av_dump_format(pFormatCtx, 0, filepath,0);






    //qDebug()<<"runner "<<m_index<<" --> pAVCodecContext "<<pAVCodecContext<<" sizeof(pAVCodecContext): "<<sizeof(*pAVCodecContext);
    pAVCodecContext = avcodec_alloc_context3(NULL);
    //qDebug()<<"runner "<<m_index<<" <-- pAVCodecContext "<<pAVCodecContext<<" sizeof(pAVCodecContext): "<<sizeof(*pAVCodecContext);

       m_pAVCodecContext->add_to_statistics(QString::number((qint64)pAVCodecContext,16));

    ////qDebug()<<"runner "<<m_index<<" pAVCodecContext: "<<pAVCodecContext;
    //qDebug()<<" ";
    //qDebug()<<"runner "<<m_index<<" --> pAVCodec "<<pAVCodec<<" sizeof(pAVCodec): "<<sizeof(*pAVCodec);
    pAVCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codec->codec_id);
    //qDebug()<<"runner "<<m_index<<" -- pAVCodec "<<pAVCodec<<" sizeof(pAVCodec): "<<sizeof(*pAVCodec);

    m_pAVCodec->add_to_statistics(QString::number((qint64)pAVCodec,16));

    prev=clock();
    pFormatCtx->interrupt_callback.callback=interrupt_cb;
    pFormatCtx->interrupt_callback.opaque = this;
    //qDebug()<<" ";
    //qDebug()<<"runner "<<m_index<<" --> param: "<<param<<" sizeof(param): "<<sizeof(*param);
    avcodec_parameters_from_context(param, pFormatCtx->streams[videoindex]->codec);
    //qDebug()<<"runner "<<m_index<<" --> param: "<<param<<" sizeof(param): "<<sizeof(*param);

        m_param->add_to_statistics(QString::number((qint64)param,16));

    avcodec_parameters_to_context(pAVCodecContext, param);
    //qDebug()<<"runner "<<m_index<<" <-- param: "<<param<<" sizeof(param): "<<sizeof(*param);
    //qDebug()<<"runner "<<m_index<<" <-- pAVCodecContext "<<pAVCodecContext<<" sizeof(pAVCodecContext): "<<sizeof(*pAVCodecContext);



    prev=clock();
    pFormatCtx->interrupt_callback.callback=interrupt_cb;
    pFormatCtx->interrupt_callback.opaque = this;
    //qDebug()<<" ";
    videoWidth=pAVCodecContext->width;
    videoHeight=pAVCodecContext->height;

  ////qDebug()<<"URL: "<<URL<<"; width: "<<videoWidth<<"; height: "<<videoHeight;

    if(videoWidth<=640 && videoHeight<=480){

       ////qDebug()<<"Этот видеопоток нужно попробовать сохранить";

    }

    //qDebug()<<"runner "<<m_index<<" --> pAVPicture: "<<pAVPicture<<" sizeof(pAVPicture): "<<sizeof(*pAVPicture);
    avpicture_alloc(pAVPicture,AV_PIX_FMT_RGB32,videoWidth,videoHeight);
    //qDebug()<<"runner "<<m_index<<" -- pAVPicture: "<<pAVPicture<<" sizeof(pAVPicture): "<<sizeof(*pAVPicture);

        m_pAVPicture->add_to_statistics(QString::number((qint64)pAVPicture,16));

    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);

    if((videoWidth==0)&&(videoHeight==0)){

        ////qDebug()<<"FAIL with: videoWidth videoHeight";
       return false;
     }
////qDebug()<<"sws_getContext pSwsContext "<<(pSwsContext==NULL);
    //qDebug()<<"runner "<<m_index<<" --> pSwsContext: "<<pSwsContext<<" sizeof(pSwsContext): "<<sizeof(pSwsContext);
    pSwsContext = sws_getContext(videoWidth,videoHeight,pAVCodecContext->pix_fmt,videoWidth,videoHeight,AV_PIX_FMT_RGB32,SWS_BICUBIC,0,0,0);
    //qDebug()<<"runner "<<m_index<<" <-- pSwsContext: "<<pSwsContext<<" sizeof(pSwsContext): "<<sizeof(pSwsContext);

    m_pSwsContext->add_to_statistics(QString::number((qint64)pSwsContext,16));

    ////qDebug()<<"runner "<<m_index<<" pSwsContext: "<<pSwsContext;
  ////qDebug()<<"sws_getContext pSwsContext "<<(pSwsContext==NULL);
//    AVBufferRef *hw_device_ctx = NULL;
    //qDebug()<<" ";
 //  pAVCodecContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    pAVCodecContext->thread_count=1;

////qDebug()<<"int result=avcodec_open2(av_context,codec,NULL);";
  //  int result=avcodec_open2(av_context,codec,NULL);
 ////qDebug()<<"profit";
    int result=avcodec_open2(pAVCodecContext,pAVCodec,&options);
    if (result<0){
        av_codec_not_open++;
 //   close();
        emit finished();

       ////qDebug()<<"FAIL with: avcodec_open2t";
        return false;
    }
av_codec_open++;
    int y_size = pAVCodecContext->width * pAVCodecContext->height;

    prev=clock();



    return true;

}

void Runner::free_settings()
{


    if(pAVPicture!=NULL){
        qDebug()<<"avpicture_free(pAVPicture)-->";
        avpicture_free(pAVPicture);
        qDebug()<<"--> delete pAVPicture";

        allocator->deallocate(pAVPicture,sizeof(AVPicture));
      //  delete pAVPicture;

        qDebug()<<"<-- delete pAVPicture";
        pAVPicture=NULL;
        *data=NULL;
        qDebug()<<"avpicture_free(pAVPicture)<--";
    }


 //qDebug()<<"freeSettings --> ";
    //qDebug()<<"pFormatContext "<<&pFormatCtx<<" "<<(pFormatCtx==NULL);
      if(pFormatCtx!=NULL){
           //qDebug()<<"pFormatCtx-->";
             //qDebug()<<"vformat_close_input(&pFormatCtx); "<<videoindex<<" pFormatCtx->nb_streams "<<pFormatCtx->nb_streams;
             if(pFormatCtx->streams) {
                 //qDebug()<<"+";
                 if(videoindex<pFormatCtx->nb_streams&&videoindex>=0){
          if(pFormatCtx->streams[videoindex]->codec){
              //qDebug()<<"avcodec_close(pFormatCtx->streams[videoindex]->codec);";
             avcodec_close(pFormatCtx->streams[videoindex]->codec);
             av_codec_close++;
          }
                 }
      }

    //  avcodec_close(pFormatCtx->streams[videoindex]->codec);
      avformat_close_input(&pFormatCtx);
      avformat_free_context(pFormatCtx);
      pFormatCtx=NULL;

                 //qDebug()<<"pFormatCtx<--";
      }

 //qDebug()<<"options "<<(options==NULL);




       //qDebug()<<"param "<<(param==NULL);



             //qDebug()<<"pAVCodecContext "<<(pAVCodecContext==NULL);

       if(pAVCodecContext!=NULL){
               //qDebug()<<" avcodec_free_context(&pAVCodecContext);";
      avcodec_free_context(&pAVCodecContext);
       }
        //qDebug()<<"freeSettings <-- ";

}
//  //qDebug()<<" ";
void Runner::free()
{
    /*
    if(packet){
     //qDebug()<<" av_free(packet); ";
     //   av_packet_unref(packet);
    av_free(packet);
    }
    */




  //qDebug()<<"pSwsContext "<<(pSwsContext==NULL);
    if(pSwsContext!=NULL){
        //qDebug()<<"sws_freeContext(pSwsContext); ";
    sws_freeContext(pSwsContext);
    pSwsContext=NULL;
    }
    //if(pAVCodecContext)
    //avcodec_free_context(&pAVCodecContext);

}

bool Runner::capture()
{

    int res=(av_read_frame(pFormatCtx, &packet));

   if(res<0){
    //   //qDebug()<<"interrupt lostConnection";

       return false;
   }

   int used=0;
   int got_frame=0;
   if (pAVCodecContext->codec_type == AVMEDIA_TYPE_VIDEO ||
              pAVCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
              used = avcodec_send_packet(pAVCodecContext, &packet);
              if (used < 0 && used != AVERROR(EAGAIN) && used != AVERROR_EOF) {
             } else {
              if (used >= 0)
                  packet.size = 0;
              used = avcodec_receive_frame(pAVCodecContext, pAVFrame);
              if (used >= 0)
                  got_frame = 1;
 //             if (used == AVERROR(EAGAIN) || used == AVERROR_EOF)
 //                 used = 0;
              }
          }

   if (got_frame==1){


           sws_scale(pSwsContext,
                    (const uint8_t* const *)pAVFrame->data,
                    pAVFrame->linesize,
                    0,
                    videoHeight,
                    pAVPicture->data,
                    pAVPicture->linesize
                    );


   //    }



         //  //qDebug()<<".";

           *h=videoHeight;
           *w=videoWidth;

           *data=pAVPicture;

           emit new_frame(URL);

           if(streamType==StreamType::Snapshot){
           //    is_over=true;
           }
/*
           *img=QImage(pAVPicture->data[0],
                       videoWidth,
                       videoHeight,
                       QImage::Format_RGB32);



           if(m_running==mode::Snapshot){
               set_m_running(Mode::doNothing;
           }
*/


   }

//   av_packet_free(&packet);
   av_packet_unref(&packet);

   return true;
}


void Runner::run()
{
    //qDebug()<<"RUN "<<m_index;
    int count=0;
    int previos=0;
    int interval=10;
    bool sleep=true;
    int frameCnt=0;
    while(m_running!=Mode::Exit){
        sleep=true;
        time=QDateTime::currentDateTime();
        if(m_running!=previos){

            //qDebug()<<"runner "<<m_index<<" m_running: "<<m_running<<" "<<get_state() ;
            if(m_running==Runner::Mode::Waiting){
                //qDebug()<<QTime::currentTime()<<" runner "<<m_index<<" начал работу с потоком типа"<<streamType;//<<URL;
                go_to_free_state=false;
            }
            if(m_running==Runner::Mode::Free){
                //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" свободен: ";//<<URL;
                go_to_free_state=false;
                first_frame_getted=false;
            }
            previos=m_running;
        }
       // //qDebug()<<"mode "<<m_running;
        if(m_running==Mode::Play || m_running==Mode::Waiting){
            if (!capture()){
                count++;
                //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" нет кадров: "<<count<<" "<<URL;
                if(count>2){
                    //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" потеря связи с потоком: "<<URL;
                    m_running=Mode::Lost;
                    count=0;
                }
                //    emit  finished();

            }else{
                frameCnt++;
                if(frameCnt>0){
                    if(m_running==Mode::Waiting){
                      //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" первый кадр ";//<<URL;
                    }
                m_running=Mode::Play;

                }
                first_frame_getted=true;
                count=0;

                if(streamType==StreamType::Snapshot){
                    //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" снимок готов ";//<<URL;
                    set_m_running(Mode::Hold);

                }

            }

        }

        //открыть новый поток
        if(frash_stream){
            losted=false;
            frameCnt=0;
            //qDebug()<<QTime::currentTime()<<" runner "<<m_index<<" новый поток: ";//<<URL;

            set_m_running(Mode::Prepare);

            frash_stream=0;
            local_mutex.lock();


            //qDebug()<<"pAVPicture: "<<(pAVPicture==NULL);
            if(pAVPicture==NULL){
             //qDebug()<<"pAVPicture = new AVPicture()-->";

                pAVPicture = allocator->allocate(sizeof(AVPicture));

             //   pAVPicture = new AVPicture();


              //   qDebug()<<"pAVPicture: "<<pAVPicture;
              //  qDebug()<<"sizeof(AVPicture): "<<sizeof(AVPicture);
             //qDebug()<<"pAVPicture = new AVPicture()<--";
            }
            //qDebug()<<"pAVPicture: "<<(pAVPicture==NULL);

            if (!load_settings()){

                //   free_settings();
                //   free();
                //   go_to_free_state=true;
                 local_mutex.unlock();
                //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" поток не открылся: ";//<<URL;
                //     emit lost_connection(URL);
                set_m_running(Mode::Lost);
            }else{

                set_m_running(Mode::Waiting);
                go_to_free_state=false;
                sleep=false;
                local_mutex.unlock();
                //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" поток открылся: ";//<<URL;


            }


        }

        if(m_running==Mode::Low){

            if(return_from_low_mode){
                return_from_low_mode=false;
                //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" освобождаем";

                set_m_running(Mode::Play);

            }
        }

        if(go_to_low_mode){
            go_to_low_mode=false;
            //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" в low mode";

            set_m_running(Mode::Low);
        }

        if(go_to_free_state){
            go_to_free_state=false;
            //qDebug()<<QDateTime::currentDateTime()<<" runner "<<m_index<<" освобождаем";
            local_mutex.lock();
            free_settings();
            free();
            local_mutex.unlock();
            set_m_running(Mode::Free);
        }


        if(m_running==Mode::Hold){
            emit new_frame(URL);
        }
        if(m_running==Mode::Lost){
                if(losted==false){
            emit lost_connection(URL);
                    losted=true;
                }
        }



        if(videoHeight>600&&
                videoWidth>800){
            interval=10;
        }else{
            interval=1000;
        }
        if(sleep){
        QThread::usleep(interval);
        }

    }
//qDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!1";
emit  finished();
    /*
    //qDebug()<<QDateTime::currentDateTime()<<"Runner::run()";
av_log_set_level(AV_LOG_QUIET);
prev=clock();

 local_mutex.lock();
load();
if (!load_settings()){
    //qDebug()<<"001";
    emit  finished();
    local_mutex.unlock();
    emit lost_connection(URL);
    return;
}
local_mutex.unlock();

prev=clock();

pFormatCtx->interrupt_callback.callback=interrupt_cb;
pFormatCtx->interrupt_callback.opaque = this;

if(m_running==Mode::LiveStreaming||m_running==Mode::StorageStreaming){
    emit playing();
}

int frame_cnt=0;

is_over=false;

while(m_running!=Mode::TurnOff){

   // //   //qDebug()<URL<<"..                                          "<<delay;
   prev=clock();

if(!is_over){
   if (!capture()){
       //qDebug()<<"002";
       emit lost_connection(URL);
       emit  finished();
       return;
   }

}


QThread::usleep(10);
}

emit finished();

return;
  */


}

void Runner::close()
{
free_settings();
free();

}








Statistic::Statistic(QString name_)
{
    name = name_;
    added=0;
    deleted=0;
}

void Statistic::show_statistic()
{
  //  qDebug()<<"cоздано "<<added<<" закрыто "<<deleted;
   qDebug()<<"statistics for "<<name<<" всего: "<<map.size();
   int reuse=0;
   int once=0;
   int i=0;
 //  qDebug()<<"первые 10 адресов:";
   foreach(auto addr, map.keys()){
       i++;
       if(i<=10){
    //     qDebug()<<"addr "<<addr<<" "<<map.value(addr);
       }
       if(map.value(addr)>1){
           reuse++;
    //   qDebug()<<": "<<addr<<" "<<map.value(addr);
       }else{
         once++;
       }
   }
       if(reuse>0){
        qDebug()<<"повторно использованных адресов  : "<<reuse;
       }
       if(once>0){
        qDebug()<<"однократно использованных адресов: "<<once;
       }
       qDebug()<<" ";

}

void Statistic::add_to_statistics(QString addr)
{
    if(map.contains(addr)){
        int val=map.value(addr)+1;
        map.insert(addr,val);
    }else{
        map.insert(addr,1);
    }
}
