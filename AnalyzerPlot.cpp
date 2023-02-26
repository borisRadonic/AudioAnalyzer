#include "AnalyzerPlot.h"
#include "frequencyplot.h"

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
#include <qwt_symbol.h>
#include <qwt_curve_fitter.h>
#include <qwt_text.h>

AnalyzerPlotCurve::AnalyzerPlotCurve( const QString& name, FreqValues* pValues, QwtPlot* pPlot, const QColor& color )
: m_strName( name )
, m_pFreqValues( pValues )
{
    m_pPlotCurve = new QwtPlotCurve( name );
    m_pPlotCurve->attach( pPlot );
    m_pPlotCurve->setPen(color);
    m_pPlotCurve->setStyle(QwtPlotCurve::Lines);
}

AnalyzerPlotCurve::~AnalyzerPlotCurve(void)
{
    delete m_pPlotCurve;
}

void AnalyzerPlotCurve::setColor( const QColor& color )
{
    m_pPlotCurve->setPen(color);
}

void AnalyzerPlotCurve::Plot()
{
    m_pPlotCurve->setSamples( m_pFreqValues->GetXValues(), m_pFreqValues->GetValues() );
}

QwtPlotCurve* AnalyzerPlotCurve::GetPlotCurve()
{
    return m_pPlotCurve;
}

AnalyzerPlotBase::AnalyzerPlotBase( QWidget *parent, FrequencyPlot* pFreqencyPlot  )
:QwtPlot( parent ),
m_pFrequencyPlot( pFreqencyPlot )
{
    setAutoReplot();
    canvas()->installEventFilter(parent);
    setCanvasBackground(QColor(Qt::black));
    QwtLegend* legend = new QwtLegend();
    insertLegend( legend, QwtPlot::TopLegend);
    //QObject::connect( this, SIGNAL(legendClicked(QwtPlotItem*) ), this, SLOT( legendClicked(QwtPlotItem *)) );
    m_markerPeak1 = new QwtPlotMarker();
    m_markerPeak1->setLineStyle( QwtPlotMarker::HLine );
    m_markerPeak1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    m_markerPeak1->setLinePen( QColor(  Qt::red ), 0, Qt::DashDotLine );
    m_markerPeak1->attach( this );

    m_markerPeak2 = new QwtPlotMarker();
    m_markerPeak2->setLineStyle( QwtPlotMarker::HLine );
    m_markerPeak2->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    m_markerPeak2->setLinePen( QColor(  Qt::red ), 0, Qt::DashDotLine );
    m_markerPeak2->attach( this );

    m_pTimer = new QTimer;
    m_pTimer->setSingleShot(true);
}

void AnalyzerPlotBase::legendClicked(QwtPlotItem * plotItem)
{
    QString name = plotItem->title().text();
    QColorDialog dlgColor( plotItem->title().color()  );
    if( dlgColor.exec() == QDialog::Accepted )
    {
        m_pFrequencyPlot->ChangeCurveColor( dlgColor.currentColor(), name );
    }
}

void AnalyzerPlotBase::showPeak( double freq1, double amplitude1, double freq2, double amplitude2 )
{
    if( freq1 == 0 )
    {
        m_markerPeak1->setValue( 0.0, 0.0 );
        m_markerPeak1->setLabel( QString("") );
    }
    else
    {
        QString label1;
        label1.sprintf( "%.3f dB", amplitude1 );

        QwtText text1( label1 );
        text1.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
        text1.setColor( QColor( Qt::red ) );

        m_markerPeak1->setValue( freq1, amplitude1 );
        m_markerPeak1->setLabel( text1);
    }

    if( freq2 == 0 )
    {
        m_markerPeak2->setValue( 0.0, 0.0 );
        m_markerPeak2->setLabel( QString("") );
    }
    else
    {
        QString label2;
        label2.sprintf( "%.3f dB", amplitude2 );

        QwtText text2( label2 );
        text2.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
        text2.setColor( QColor( Qt::red ) );

        m_markerPeak2->setValue( freq2, amplitude2 );
        m_markerPeak2->setLabel( text2 );
    }
}



