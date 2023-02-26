#ifndef ANALOGPLOT_H
#define ANALOGPLOT_H

#include "scopeplot.h"
#include "scopeplotbase.h"

#include <vector>
#include <string>
#include <QObject>
#include <QColor>
#include <qwt_plot.h>

class QwtPlotMarker;
class ScopePlotCurve;
class AnalogSignal;


class AnalogPlot : public ScopePlot
{

public:

    AnalogPlot( QWidget *parent , const QString& name );

    virtual ~AnalogPlot(void);

    void AddAnalogSignal( AnalogSignal* pAnalogSignal, const QColor& color, QString& name  );

    void RemoveAnalogSignal( const QString& name );

    typedef std::vector<ScopePlotCurve*>            VEC_ANALOG_SCOPE_PLOT_CURVE_PTR;

    typedef std::vector<ScopePlotCurve*>::iterator  VEC_ANALOG_SCOPE_PLOT_CURVE_PTR_ITER;

    void setTimeBase( const unsigned int timeBaseInUs );

    virtual void PlotSignals();

    virtual void PlotLine();

    inline QwtPlot * GetPlotWidget()
    {
        return (QwtPlot*) m_pPlot;
    }

    void ChangeCurveColor(const QColor& color, const QString& name);

    void EnableZoomer (void);

    ScopePlotCurve* GetPlotCurveFromName( const QString& name);

protected:

    VEC_ANALOG_SCOPE_PLOT_CURVE_PTR m_vecPtrScopePlotCurve;

    QString m_strName;

    ScopePlotBase *m_pPlot;

    QwtPlotMarker* m_pPlotMarker;

    //AnalogPlotZoomer* m_pPlotZoomer;
};


#endif // ANALOGPLOT_H
