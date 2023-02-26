#ifndef IANALYZER_H
#define IANALYZER_H


#include <QVector>

class IAnalyzer
{
public:

    virtual ~IAnalyzer()
    {
    }

    virtual void insertAnalyzerData( const QVector<double>& channel1, const QVector<double>& channel2 ) = 0;

};


#endif // IANALYZER_H
