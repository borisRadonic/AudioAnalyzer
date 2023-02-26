#ifndef AUDIOINTERFACE_H
#define AUDIOINTERFACE_H

#include "signalgenerator.h"
#include "iscope.h"
#include "IAnalyzer.h"
#include "rtaudio\RtAudio.h"

int AudioInterfaceInOut( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );

class AudioInterface : public SignalGenerator
{

public:


    explicit AudioInterface( RtAudio* audioInterface, unsigned int sampleRate, unsigned int inputDevice, unsigned int outputDevice, QObject *parent = 0);

    void startAudio( RtAudio* audioInterface, unsigned int sampleRate );

    void stopAudio();

    inline void connectScope( IScope* scope )
    {
        m_scope = scope;
    }

    inline void disconnectScope()
    {
        m_scope = NULL;
    }

    inline IScope* scope()
    {
        return m_scope;
    }

    inline void connectAnalyzer( IAnalyzer* analyzer )
    {
        m_analyzer = analyzer;
    }

    inline void disconnectAnalyzer()
    {
        m_analyzer = NULL;
    }

    inline IAnalyzer* analyzer()
    {
        return m_analyzer;
    }

    inline void setOut1( bool value )
    {
        m_out1 = value;
    }

    inline void setOut2( bool value )
    {
        m_out2 = value;
    }

    inline bool isOut1()
    {
        return m_out1;
    }

    inline bool isOut2()
    {
        return m_out2;
    }

    inline unsigned int sampleRate()
    {
        return m_sampleRate;
    }

private:


    int             m_started;
    RtAudio*        m_ptrAudioInterace;


    unsigned int    m_sampleRate;
    unsigned int    m_inputDevice;
    unsigned int    m_outputDevice;

    IScope*         m_scope;
    IAnalyzer*      m_analyzer;

    bool            m_out1;
    bool            m_out2;

    AudioInterface()
    {
    }
};


#endif // AUDIOINTERFACE_H
