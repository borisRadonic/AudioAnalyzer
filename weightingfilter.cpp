#include "weightingfilter.h"
#include "math.h"

WeightingFilterItuR468::WeightingFilterItuR468():WeightingFilter()
{
    PAIR pair;

    pair.freq = 0;
    pair.amp = -38;
    m_table.append( pair );

    pair.freq = 31.5;
    pair.amp = -29.9;
    m_table.append( pair );

    pair.freq = 63;
    pair.amp = -23.9;
    m_table.append( pair );

    pair.freq = 100;
    pair.amp = -19.8;
    m_table.append( pair );

    pair.freq = 200;
    pair.amp = -13.8;
    m_table.append( pair );

    pair.freq = 400;
    pair.amp = -7.8;
    m_table.append( pair );

    pair.freq = 800;
    pair.amp = -1.9;
    m_table.append( pair );

    pair.freq = 1000;
    pair.amp  = 0.0;
    m_table.append( pair );

    pair.freq = 2000;
    pair.amp =  5.6;
    m_table.append( pair );

    pair.freq = 3150;
    pair.amp =  9.0;
    m_table.append( pair );

    pair.freq = 4000;
    pair.amp  = 10.5;
    m_table.append( pair );

    pair.freq = 5000;
    pair.amp = -11.7;
    m_table.append( pair );

    pair.freq = 6300;
    pair.amp  = 12.2;
    m_table.append( pair );

    pair.freq = 7100;
    pair.amp  = 12;
    m_table.append( pair );

    pair.freq = 8000;
    pair.amp  = 11.4;
    m_table.append( pair );

    pair.freq = 9000;
    pair.amp  = 10.1;
    m_table.append( pair );

    pair.freq = 10000;
    pair.amp  = 8.1;
    m_table.append( pair );

    pair.freq = 12500;
    pair.amp  = 0;
    m_table.append( pair );

    pair.freq = 14000;
    pair.amp = -5.3;
    m_table.append( pair );

    pair.freq = 16000;
    pair.amp = -11.7;
    m_table.append( pair );

    pair.freq = 20000;
    pair.amp = -22.2;
    m_table.append( pair );

    pair.freq = 31500;
    pair.amp = -42.7;
    m_table.append( pair );

}

double WeightingFilterItuR468::getMagnitudeIndB( const double frequency )
{
    double f1(0.0);
    double f2(0.0);
    double a1(0.0);
    double a2(0.0);
    for( unsigned int i = 0; i < m_table.size(); i++ )
    {
        if( m_table.at(i).freq <= frequency )
        {
            f1 = m_table.at(i).freq;
            a1 = m_table.at(i).amp;
        }
        if( m_table.at(i).freq >= frequency )
        {
            f2 = m_table.at(i).freq;
            a2 = m_table.at(i).amp;
            break;
        }
    }
    if( f1 == f2 )
    {
        return a1;
    }
    double amp =  a1 + (frequency-f1)* ((a2-a1)/(f2-f1));
    return amp;
}

double WeightingFilterItuR468::WeightingFilterItuR468::getMagnitude( const double frequency )
{
    double m = getMagnitudeIndB( frequency ) / 20;
    return pow( 10, m );
}

