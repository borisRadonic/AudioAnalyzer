#ifndef ANALYZERPLOT_H
#define ANALYZERPLOT_H

#include <vector>
#include <string>
#include <QObject>
#include <QColor>
#include <qwt_plot.h>
#include <string>
#include <QColor>

#include "SignalBuffer.h"

class QWidget;
class FrequencyPlot;
class QTimer;
class ScopePlotCurve;
class QwtPlotMarker;
class QwtPlotCurve;
class QwtPlot;
class QwtPlotMarker;



class FreqValues : public CSignalBuffer
{
public:
    FreqValues():CSignalBuffer()
    {
    }

    ~FreqValues(void)
    {
    }
};


class AnalyzerPlotBase : public QwtPlot
{

public:

    explicit  AnalyzerPlotBase( QWidget *parent, FrequencyPlot* pFreqencyPlot );

    ~AnalyzerPlotBase(){}

    void showPeak( double freq1, double amplitude1, double freq2, double amplitude2 );

protected slots:

    void legendClicked(QwtPlotItem * plotItem);

    void legendChecked(QwtPlotItem * plotItem, bool on);

private:

    FrequencyPlot* m_pFrequencyPlot;

    QTimer* m_pTimer;

    QwtPlotMarker*  m_markerPeak1;
    QwtPlotMarker*  m_markerPeak2;
};


class AnalyzerPlotCurve
{
public:

    AnalyzerPlotCurve( const QString& name, FreqValues* pValues, QwtPlot* pPlot, const QColor& color );

    ~AnalyzerPlotCurve(void);

    inline const QString Name()
    {
        return m_strName;
    }

    void Plot();

    inline FreqValues* GetValues()
    {
        return m_pFreqValues;
    }

    void setColor( const QColor& color );

    QwtPlotCurve* GetPlotCurve();


protected:

    QString         m_strName;
    QwtPlotCurve*   m_pPlotCurve;
    FreqValues*     m_pFreqValues;


};



#endif // ANALYZERPLOT_H
