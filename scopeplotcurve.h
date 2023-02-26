#ifndef SCOPEPLOTCURVE_H
#define SCOPEPLOTCURVE_H

#include <string>
#include <QColor>

class QwtPlotCurve;
class AnalogSignal;
class QwtPlot;

class ScopePlotCurve
{
public:

    ScopePlotCurve( const QString& name, AnalogSignal* pAnalogSignal, QwtPlot* pPlot, const QColor& color );

    ~ScopePlotCurve(void);

    inline const QString Name()
    {
        return m_strName;
    }

    void Plot();

    inline AnalogSignal* GetAnalogSignal()
    {
        return m_pAnalogSignal;
    }

    void setColor( const QColor& color );

    QwtPlotCurve* GetPlotCurve();

protected:

    QString         m_strName;
    QwtPlotCurve*   m_pPlotCurve;
    AnalogSignal*   m_pAnalogSignal;

};


#endif // SCOPEPLOTCURVE_H
