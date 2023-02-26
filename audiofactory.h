#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "rtaudio\RtAudio.h"


class AudioFactory
{

public:

    static RtAudio* CreateAudio();

private:

    AudioFactory()
    {
    }
};
#endif // AUDIOOUTPUT_H
