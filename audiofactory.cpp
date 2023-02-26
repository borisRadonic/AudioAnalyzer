#include "audiofactory.h"
#include <QDebug>
#include "RtAudio/RtAudio.h"

RtAudio* AudioFactory::CreateAudio()
{
    RtAudio::Api api = RtAudio::WINDOWS_ASIO;
    // outputDevice, outputChannels, inputDevice, inputChannels, format, sampleRate, bufferSize, nBuffers
    RtAudio* audio = NULL;

    try
    {
        audio = new RtAudio( api  );
    }
    catch ( RtAudioError& e )
    {
        e.printMessage();
    }


    return audio;
}
