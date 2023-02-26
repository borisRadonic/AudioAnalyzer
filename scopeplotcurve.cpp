
#include "scopeplotcurve.h"
#include "AnalogSignal.h"
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_curve_fitter.h>
#include <qwt_text.h>

ScopePlotCurve::ScopePlotCurve( const QString& name, AnalogSignal* pAnalogSignal, QwtPlot* pPlot, const QColor& color )
: m_strName( name )
, m_pAnalogSignal( pAnalogSignal )
{
    m_pPlotCurve = new QwtPlotCurve( name );
    m_pPlotCurve->attach( pPlot );
    m_pPlotCurve->setPen(color);
    m_pPlotCurve->setStyle(QwtPlotCurve::Lines);
}

ScopePlotCurve::~ScopePlotCurve(void)
{
    delete m_pPlotCurve;
}

void ScopePlotCurve::setColor( const QColor& color )
{
    m_pPlotCurve->setPen(color);
}

void ScopePlotCurve::Plot()
{
    m_pPlotCurve->setSamples( m_pAnalogSignal->GetXValues(), m_pAnalogSignal->GetValues() );
}

QwtPlotCurve* ScopePlotCurve::GetPlotCurve()
{
    return m_pPlotCurve;
}
