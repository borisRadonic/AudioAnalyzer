#include "signalgenerator.h"
#include <qmath.h>
#include <QDebug>
#include "randomnumber.h"

SignalGenerator::SignalGenerator()
:m_frequency1(60)
,m_frequency2(7000)
,m_signalType(SignalGenerator::eSinus)
,m_amp1(1.0/1.778) //-17db
,m_amp2(1.0/7.0794) //-5db
,m_phi1(0.0)
,m_phi2(0.0)
,m_cycle(0)
{
    m_randomGen = new RandomNumber();
}

SignalGenerator::~SignalGenerator(void)
{
    delete m_randomGen;
}

double SignalGenerator::compute( unsigned int sampleRate )
{
    double x(0.0);
    double  time = (1.0/(double) sampleRate) * m_cycle ;
    double freq = (double) m_frequency1;
    double freq2 = (double) m_frequency2;
    static double max(0.0);
    if( m_signalType == SignalGenerator::eSinus)
    {
        x = m_amp1 * sin(  2.0 * M_PI *  freq * time +  m_phi1);
    }
    else  if( m_signalType == SignalGenerator::eSinPlusSin)
    {
        x = m_amp1 * sin(  2.0 * M_PI * freq  * time +  m_phi1) +
            m_amp2 * sin(  2.0 * M_PI * freq2 * time +  m_phi2);
    }
    else  if( m_signalType == SignalGenerator::eSNTest)
    {
        x = 0.0;
    }
    else  if( m_signalType == SignalGenerator::eWhiteNoise)
    {
        x = m_amp1 * 2 * m_randomGen->white( 0.5 );
    }
    else  if( m_signalType == SignalGenerator::ePinkNoise)
    {
        //x=
    }
    else  if( m_signalType == SignalGenerator::eSweep)
    {
        //x=
    }
    ++m_cycle;
    if( x > max )
    {
        max = x;
        qDebug() << x;
    }
    return x;
}
