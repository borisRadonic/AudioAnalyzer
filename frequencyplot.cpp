
#include "frequencyplot.h"
#include "analyzerplot.h"
#include <QWidget>
#include <qwt_scale_map.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_math.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qframe.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_canvas.h>
#include <QDebug>
#include <QTimer>
#include <QColorDialog>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_text.h>
#include "qwt_scale_engine.h"

FrequencyPlot::FrequencyPlot (QWidget *parent , const QString& name)
    :m_strName( name )
{
    m_pPlot = new AnalyzerPlotBase( parent, this );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );

    m_pPlot->setCanvas( canvas );
    m_pPlot->setCanvasBackground( QColor( "Black" ) );

    // legend
    //QwtLegend *legend = new QwtLegend;
    //m_pPlot->insertLegend( legend, QwtPlot::BottomLegend );

    // axes
    m_pPlot->setAxisTitle( QwtPlot::xBottom, "Frequency [Hz]" );
    m_pPlot->setAxisTitle( QwtPlot::yLeft, "Amplitude [dB]" );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setMajorPen( QColor::fromRgb( 255,255,255), 0.3, Qt::DotLine);
    grid->setMinorPen( QColor::fromRgb( 255,255,255), 0.3, Qt::DotLine);
    grid->attach( (QwtPlot*) m_pPlot);
    m_pPlot->setAxisScale(QwtPlot::xBottom, 20, 20000, 1 );
    m_pPlot->setAxisScale( QwtPlot::yLeft, -140.0, 10, 20 );
    m_pPlot->enableAxis(  QwtPlot::xBottom, true );
    m_pPlot->enableAxis(  QwtPlot::yLeft, true );

    m_pPlot->setAxisMaxMinor( QwtPlot::xBottom, 10 );
    m_pPlot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    m_pPlot->resize( parent->width(), parent->height() );


    m_pPlot->replot();
}

FrequencyPlot::~FrequencyPlot(void)
{
    for ( VEC_ANALYZER_PLOT_CURVE_PTR_ITER it = m_vecPtrAnalyzerPlotCurve.begin() ; it < m_vecPtrAnalyzerPlotCurve.end(); it++ )
    {
        AnalyzerPlotCurve* pPlotCurve = *it;
        delete pPlotCurve;
    }
}

void FrequencyPlot::AddFreqValue( FreqValues* pFreqValues, const QColor& color, QString& name  )
{
    AnalyzerPlotCurve* pPlotCurve  = new AnalyzerPlotCurve( name, pFreqValues, (QwtPlot*) m_pPlot, color );

    m_vecPtrAnalyzerPlotCurve.push_back( pPlotCurve );
}

void FrequencyPlot::changeFreqRange( int fStart, int fStop )
{
    m_pPlot->setAxisScale(QwtPlot::xBottom, (double) fStart, (double) fStop, 1 );
    m_pPlot->replot();
}

void FrequencyPlot::Plot()
{
    for ( VEC_ANALYZER_PLOT_CURVE_PTR_ITER it = m_vecPtrAnalyzerPlotCurve.begin() ; it < m_vecPtrAnalyzerPlotCurve.end(); it++ )
    {
        AnalyzerPlotCurve* pPlotCurve = *it;
        pPlotCurve->Plot();
    }
}

void FrequencyPlot::ChangeCurveColor(const QColor& color, const QString& name)
{
    for ( VEC_ANALYZER_PLOT_CURVE_PTR_ITER it = m_vecPtrAnalyzerPlotCurve.begin() ; it < m_vecPtrAnalyzerPlotCurve.end(); it++ )
    {
        AnalyzerPlotCurve* pPlotCurve = *it;
        if( pPlotCurve->Name() == name )
        {
            pPlotCurve->setColor( color );
            return;
        }
    }
}

AnalyzerPlotCurve* FrequencyPlot::GetPlotCurveFromName( const QString& name)
{
    for ( VEC_ANALYZER_PLOT_CURVE_PTR_ITER it = m_vecPtrAnalyzerPlotCurve.begin() ; it < m_vecPtrAnalyzerPlotCurve.end(); it++ )
    {
        AnalyzerPlotCurve* pPlotCurve = *it;
        if( pPlotCurve->Name() == name )
        {
            return pPlotCurve;
        }
    }
    return NULL;
}

void FrequencyPlot::showPeak( double freq1, double amplitude1, double freq2, double amplitude2 )
{
    m_pPlot->showPeak( freq1, amplitude1, freq2, amplitude2 );
}
