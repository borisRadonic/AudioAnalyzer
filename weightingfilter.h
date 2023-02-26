#ifndef WEIGHTINGFILTER_H
#define WEIGHTINGFILTER_H

#include <QVector>

class WeightingFilter
{
public:

    struct PAIR
    {
        PAIR():freq(0.0),amp(0.0)
        {
        }
        double freq;
        double amp;
    };

    WeightingFilter() {}

    virtual ~WeightingFilter() {}

    virtual double getMagnitudeIndB( const double frequency ) = 0;

    virtual double getMagnitude( const double frequency ) = 0;

protected:

    QVector<PAIR> m_table;
};


class WeightingFilterNoFilter : public WeightingFilter
{
public:

    WeightingFilterNoFilter():WeightingFilter()
    {

    }

    virtual ~WeightingFilterNoFilter() {}

    inline virtual double getMagnitudeIndB( const double frequency )
    {
        return 0.0;
    }

     inline virtual double getMagnitude( const double frequency )
     {
        return 1.0;
     }
};

class WeightingFilterItuR468 : public WeightingFilter
{
public:

    WeightingFilterItuR468();

    virtual ~WeightingFilterItuR468() {}

    virtual double getMagnitudeIndB( const double frequency );

    virtual double getMagnitude( const double frequency );
};


#endif // WEIGHTINGFILTER_H
