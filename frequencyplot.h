#ifndef FREQUENCYPLOT_H
#define FREQUENCYPLOT_H


#include "AnalyzerPlot.h"

#include <vector>
#include <string>
#include <QObject>
#include <QColor>
#include <qwt_plot.h>

class QwtPlotMarker;
class ScopePlotCurve;
class AnalogSignal;


class FrequencyPlot
{

public:

    FrequencyPlot( QWidget *parent , const QString& name );

    virtual ~FrequencyPlot(void);

    void AddFreqValue( FreqValues* pFreqValues, const QColor& color, QString& name  );

    void changeFreqRange( int fStart, int fStop );

    typedef std::vector<AnalyzerPlotCurve*>            VEC_ANALYZER_PLOT_CURVE_PTR;

    typedef std::vector<AnalyzerPlotCurve*>::iterator  VEC_ANALYZER_PLOT_CURVE_PTR_ITER;

    virtual void Plot();

    inline QwtPlot * GetPlotWidget()
    {
        return (QwtPlot*) m_pPlot;
    }

    void ChangeCurveColor(const QColor& color, const QString& name);


    AnalyzerPlotCurve* GetPlotCurveFromName( const QString& name);

    void showPeak( double freq1, double amplitude1, double freq2, double amplitude2 );

protected:

    VEC_ANALYZER_PLOT_CURVE_PTR m_vecPtrAnalyzerPlotCurve;

    QString m_strName;

    AnalyzerPlotBase *m_pPlot;

};



#endif // FREQUENCYPLOT_H
