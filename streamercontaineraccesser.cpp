#include "streamercontaineraccesser.h"
QSharedPointer<StreamerContainer> StreamerContainerAccesser::streamer;
StreamerContainerAccesser::StreamerContainerAccesser()
{

}

QSharedPointer<StreamerContainer> StreamerContainerAccesser::get()
{
    if(!streamer){
        streamer=QSharedPointer<StreamerContainer>::create();
    }

    return streamer;

}
