#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <QObject>
#include <QThread>

class QTimer;
class RtAudio;
class QIODevice;
class RandomNumber;

class SignalGenerator
{
public:

    enum SignalType
    {
        eSinus          = 0,
        eSinPlusSin     = 1,
        eSNTest         = 2,
        eWhiteNoise     = 3,
        ePinkNoise      = 4,
        eSweep          = 5,
        eEnd            = 6
    };

    SignalGenerator();

    virtual ~SignalGenerator(void);


    double compute( unsigned int sampleRate );

    inline void setFrequency1( int value )
    {
        m_frequency1 = value;
    }

    inline void setFrequency2( int value )
    {
        m_frequency2 = value;
    }

    inline void setSignalType( SignalType value )
    {
        m_signalType = value;
    }

    inline void setAmp1( double value )
    {
        m_amp1 = value;
    }

    inline void setAmp2( double value )
    {
        m_amp2 = value;
    }

    inline void setPhi1( double value )
    {
        m_phi1 = value;
    }
    inline void setPhi2( double value )
    {
        m_phi2 = value;
    }

protected:

    int             m_frequency1;
    int             m_frequency2;
    SignalType      m_signalType;
    double          m_amp1;
    double          m_amp2;
    double          m_phi1;
    double          m_phi2;
    unsigned int    m_cycle;    
    RandomNumber*   m_randomGen;

};

#endif // SIGNALGENERATOR_H
