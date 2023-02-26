#include "analogplot.h"
#include "scopeplot.h"
#include "AnalogSignal.h"
#include "scopeplotcurve.h"
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


AnalogPlot::AnalogPlot (QWidget *parent , const QString& name)
: ScopePlot()
, m_strName( name )
{
    m_pPlot = new ScopePlotBase( parent, this );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );

    m_pPlot->setCanvas( canvas );
    m_pPlot->setCanvasBackground( QColor( "Black" ) );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setMajorPen( QColor::fromRgb( 255,255,255), 0.3, Qt::SolidLine);
    grid->setMinorPen( QColor::fromRgb( 255,255,255), 0.3, Qt::DotLine);
    grid->attach( (QwtPlot*) m_pPlot);
    m_pPlot->setAxisScale(QwtPlot::xBottom, 0, 10 );
    m_pPlot->setAxisScale( QwtPlot::yLeft, -1.0, 1.0, 0.25 );
    m_pPlot->enableAxis(  QwtPlot::xBottom, false );
    m_pPlot->enableAxis(  QwtPlot::yLeft, false );    
    m_pPlot->resize( parent->width(), parent->height() );

    //m_pPlotZoomer = new AnalogPlotZoomer (m_pPlot->canvas(), false);
    //m_pPlotZoomer->setEnabled(false);

    m_pPlotMarker = new QwtPlotMarker();
    m_pPlotMarker->setLineStyle( QwtPlotMarker::HLine );
    m_pPlotMarker->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    m_pPlotMarker->setLinePen( QColor( Qt::red ), 0, Qt::DashDotLine );
    m_pPlotMarker->attach( m_pPlot );


    m_pPlot->replot();
}

AnalogPlot::~AnalogPlot(void)
{
    for ( VEC_ANALOG_SCOPE_PLOT_CURVE_PTR_ITER it = m_vecPtrScopePlotCurve.begin() ; it < m_vecPtrScopePlotCurve.end(); it++ )
    {
        ScopePlotCurve* pPlotCurve = *it;
        delete pPlotCurve;
    }

    //delete m_pPlotZoomer;
    //m_pPlotZoomer = NULL;
}

void AnalogPlot::setTimeBase( const unsigned int timeBaseInUs )
{
    m_timeBase = timeBaseInUs;
    QFont font;
    font.setBold(false);
    font.setPointSize(10);
    QwtText text;
    text.setFont( font );
    //text.setColor( QColor::fromRgb( 0, 255, 0));
    switch( timeBaseInUs )
    {
        case 1:
        {
            text.setText( "1 us/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 10:
        {
            text.setText( "10 us/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 50:
        {
            text.setText( "50 us/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 100:
        {
            text.setText( "100 us/div" );
             m_pPlot->setTitle(text );
            break;
        }
        case 1000:
        {
            text.setText( "1 ms/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 5000:
        {
            text.setText( "5 ms/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 10000:
        {
            text.setText( "10 ms/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 20000:
        {
            text.setText( "20 ms/div" );
            m_pPlot->setTitle(text );
            break;
        }
        case 50000:
        {
            text.setText( "50 ms/div" );
            m_pPlot->setTitle(text );
            break;
        }      
    }
    m_pPlot->replot();
 }


void AnalogPlot::AddAnalogSignal( AnalogSignal* pAnalogSignal, const QColor& color, QString& name )
{
    ScopePlotCurve* pPlotCurve  = new ScopePlotCurve( name, pAnalogSignal, (QwtPlot*) m_pPlot, color );

    m_vecPtrScopePlotCurve.push_back( pPlotCurve );
}

void AnalogPlot::EnableZoomer( void )
{
    //m_pPlotZoomer->setEnabled(true);
}

void AnalogPlot::RemoveAnalogSignal( const QString& name )
{
    for ( VEC_ANALOG_SCOPE_PLOT_CURVE_PTR_ITER it = m_vecPtrScopePlotCurve.begin() ; it < m_vecPtrScopePlotCurve.end(); it++ )
    {
        ScopePlotCurve* pPlotCurve = *it;
        if( pPlotCurve->Name() == name )
        {
            it = m_vecPtrScopePlotCurve.erase( it );
            delete pPlotCurve;
        }
    }
}

void AnalogPlot::PlotSignals()
{
    for ( VEC_ANALOG_SCOPE_PLOT_CURVE_PTR_ITER it = m_vecPtrScopePlotCurve.begin() ; it < m_vecPtrScopePlotCurve.end(); it++ )
    {
        ScopePlotCurve* pPlotCurve = *it;
        pPlotCurve->Plot();
    }
    //m_pPlot->replot();
}

void AnalogPlot::PlotLine()
{
    QPoint pos (m_xLineValue, 0);

    if (m_vecPtrScopePlotCurve.size() > 0)
    {
        //QwtPlotCurve* pPlotCurve = m_vecPtrScopePlotCurve.at(0)->GetPlotCurve();

        //double dMax =_pPlotCurve->maxXValue();
        //double dMin = pPlotCurve->minXValue();
        //double value = ((dMax - dMin) / m_pPlot->width() * pos.x()) + dMin;

        //m_pPlotMarker->setValue(value, 0);
        m_pPlot->replot();
    }
}

void AnalogPlot::ChangeCurveColor(const QColor& color, const QString& name)
{
    for ( VEC_ANALOG_SCOPE_PLOT_CURVE_PTR_ITER it = m_vecPtrScopePlotCurve.begin() ; it < m_vecPtrScopePlotCurve.end(); it++ )
    {
        ScopePlotCurve* pPlotCurve = *it;
        if( pPlotCurve->Name() == name )
        {
            pPlotCurve->setColor( color );
            return;
        }
    }
}

ScopePlotCurve* AnalogPlot::GetPlotCurveFromName( const QString& name)
{
    for ( VEC_ANALOG_SCOPE_PLOT_CURVE_PTR_ITER it = m_vecPtrScopePlotCurve.begin() ; it < m_vecPtrScopePlotCurve.end(); it++ )
    {
        ScopePlotCurve* pPlotCurve = *it;
        if( pPlotCurve->Name() == name )
        {
            return pPlotCurve;
        }
    }
    return NULL;
}
