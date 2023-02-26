#ifndef SIGNALBUFFER_H
#define SIGNALBUFFER_H

#include <QDebug>

#include <string>
#include <memory>


//we use max 2000 positions ( the rest is a reserve to avoid swaping )
//#define BUFFER_SIZE 2010


//#define BUFFER_SIZE_RESERVE 100000
//#define MAXDOUBLE 9.9e99

class CSignalBuffer
{

public:

    CSignalBuffer()
    {
    }

    ~CSignalBuffer(void)
    {
    }

    inline unsigned int GetBufferLen()
    {
        return m_xValues.length();
    }

    inline const QVector<double>& GetXValues()
    {
        return  m_xValues;
    }

    inline const QVector<double>& GetValues()
    {
        return m_yValues;
    }

    void addValues( const QVector<double>& xValues, const QVector<double>& yValues )
    {      
        //m_xValues.clear();
        //m_yValues.clear();
        //m_xValues.reserve( xValues.length() );
        //m_yValues.reserve( yValues.length() );
        m_xValues =  xValues;
        m_yValues =  yValues;
    }

protected:

   QVector<double> m_xValues;
   QVector<double> m_yValues;
};



#endif // SIGNALBUFFER_H
