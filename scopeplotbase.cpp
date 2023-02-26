#include "scopeplotbase.h"
#include "scopeplot.h"
#include "analogplot.h"
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
//#include "AnalogPlotZoomer.h"

ScopePlotBase::ScopePlotBase( QWidget *parent, AnalogPlot* pAnalogPlot  )
:QwtPlot( parent ),
m_pAnalogPlot( pAnalogPlot )
{
    setAutoReplot();
    canvas()->installEventFilter(parent);
    setCanvasBackground(QColor(Qt::black));
    QwtLegend* legend = new QwtLegend();
    //legend->setItemMode(QwtLegend::CheckableItem);
    insertLegend( legend, QwtPlot::TopLegend);
    //QObject::connect( this, SIGNAL(legendClicked(QwtPlotItem*) ), this, SLOT( legendClicked(QwtPlotItem *)) );
    //QObject::connect( this, SIGNAL(legendChecked(QwtPlotItem*, bool) ), this, SLOT( legendChecked(QwtPlotItem *, bool)) );

    m_pTimer = new QTimer;
    m_pTimer->setSingleShot(true);
}

void ScopePlotBase::legendClicked(QwtPlotItem * plotItem)
{
    QString name = plotItem->title().text();
    QColorDialog dlgColor( plotItem->title().color()  );
    if( dlgColor.exec() == QDialog::Accepted )
    {
        m_pAnalogPlot->ChangeCurveColor( dlgColor.currentColor(), name );
    }
}

void ScopePlotBase::legendChecked(QwtPlotItem * plotItem, bool on)
{
    QString name = plotItem->title().text();

    if (m_pTimer->isActive())
    {
        m_pTimer->stop();
        legendClicked(plotItem);
    }
    else
    {
        m_pTimer->start(300);
    }

    ScopePlotCurve* pCurve = m_pAnalogPlot->GetPlotCurveFromName(name);
    if (on)
    {
        pCurve->GetPlotCurve()->hide();
    }
    else
    {
        pCurve->GetPlotCurve()->show();
    }
}
