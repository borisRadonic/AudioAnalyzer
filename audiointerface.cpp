#include "audiointerface.h"
#include <QTimer>
#include <QDebug>


AudioInterface::AudioInterface( RtAudio* audioInterface, unsigned int sampleRate, unsigned int inputDevice, unsigned int outputDevice, QObject* /*parent*/ )
    :SignalGenerator()
    ,m_started(false)
    ,m_ptrAudioInterace(audioInterface)
    ,m_sampleRate(sampleRate)
    ,m_inputDevice(inputDevice)
    ,m_outputDevice(outputDevice)
    ,m_scope(NULL)
    ,m_analyzer(NULL)
    ,m_out1(false)
    ,m_out2(false)
{
}

void AudioInterface::startAudio( RtAudio* audioInterface, unsigned int sampleRate )
{
    m_ptrAudioInterace  = audioInterface;
    m_sampleRate        = sampleRate;
    if( m_ptrAudioInterace )
    {
        m_started       = true;
        m_cycle         = 0;

        // Set the same number of channels for both input and output.
        unsigned int bufferFrames = 16384;
        RtAudio::StreamParameters iParams, oParams;
        iParams.deviceId = m_inputDevice;
        iParams.nChannels = 2;
        oParams.deviceId = m_outputDevice;
        oParams.nChannels = 2;

        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_SCHEDULE_REALTIME;
        try
        {
            if( !m_ptrAudioInterace->isStreamOpen())
            {
                m_ptrAudioInterace->openStream( &oParams, &iParams, RTAUDIO_FLOAT64, m_sampleRate, &bufferFrames, &AudioInterfaceInOut, (void*) this );
            }
        }
        catch ( RtAudioError& e )
        {
            e.printMessage();
            exit( 0 );
        }
        m_ptrAudioInterace->startStream();
    }
}

void AudioInterface::stopAudio()
{
    if( m_ptrAudioInterace && m_started )
    {
        m_started = false;
        m_ptrAudioInterace->stopStream();
        if( m_scope )
        {
            QVector<double> channel1;
            QVector<double> channel2;
            m_scope->insertScopeData( channel1, channel2 );
        }
        if( m_analyzer )
        {
            QVector<double> channel1;
            QVector<double> channel2;
            m_analyzer->insertAnalyzerData( channel1, channel2 );
        }
    }
}


// Pass-through function.
int AudioInterfaceInOut( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
    AudioInterface* ai = (AudioInterface*) data;
    double* outBuffer = (double*) outputBuffer;
    double* inBuffer = (double*) inputBuffer;
    QVector<double> channel1;
    channel1.reserve(nBufferFrames);
    QVector<double> channel2;
    channel1.reserve(nBufferFrames);
    if( status == RTAUDIO_INPUT_OVERFLOW )
    {
        qDebug() << "RTAUDIO_INPUT_OVERFLOW";
    }
    else if( status == RTAUDIO_OUTPUT_UNDERFLOW )
    {
        qDebug() << "RTAUDIO_OUTPUT_UNDERFLOW";
    }

    for( unsigned int i = 0; i < nBufferFrames; i++ )
    {
        double x = ai->compute( ai->sampleRate() );
        double v = *inBuffer;
        for ( int j= 0;  j< 2; j++ )
        {     
            if( j == 0 )
            {
                if( ai->isOut1() )
                {
                    *outBuffer = x;
                }
                else
                {
                    *outBuffer = 0;
                }
                channel1.push_back( (double) v );
            }
            else
            {
                if( ai->isOut2() )
                {
                    *outBuffer = x;
                }
                else
                {
                    *outBuffer = 0;
                }
                channel2.push_back( (double) v );
            }
            outBuffer++;
            inBuffer++;
        }
    }
    IScope* scope = ai->scope();
    if( scope )
    {
        scope->insertScopeData( channel1, channel2 );
    }
    IAnalyzer* analyzer = ai->analyzer();
    if( analyzer )
    {
        analyzer->insertAnalyzerData( channel1, channel2 );
    }
    return 0;
}





