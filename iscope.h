#ifndef ISCOPE_H
#define ISCOPE_H

#include <QVector>

class IScope
{
public:

    virtual ~IScope()
    {
    }

    virtual void insertScopeData( const QVector<double>& channel1, const QVector<double>& channel2 ) = 0;

    virtual void setSampleTime( double time ) = 0;

    virtual void connectToScope() = 0;

    virtual void disconnectFromScope() = 0;

};

#endif // ISCOPE_H
