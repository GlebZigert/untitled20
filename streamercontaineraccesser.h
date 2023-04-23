#ifndef STREAMERCONTAINERACCESSER_H
#define STREAMERCONTAINERACCESSER_H

#include "StreamerContainer.h"


class StreamerContainerAccesser
{
public:
    StreamerContainerAccesser();

    static QSharedPointer<StreamerContainer> get();

private:
   static QSharedPointer<StreamerContainer> streamer;
};

#endif // STREAMERCONTAINERACCESSER_H
