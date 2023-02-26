#ifndef SCOPEPLOTBASE_H
#define SCOPEPLOTBASE_H

#include <vector>
#include <string>
#include <QObject>
#include <QColor>
#include <qwt_plot.h>

class AnalogSignal;
class QWidget;
class AnalogPlot;
class QTimer;


class ScopePlotBase : public QwtPlot
{

    //Q_OBJECT

public:

    explicit  ScopePlotBase( QWidget *parent, AnalogPlot* pAnalogPlot );

    ~ScopePlotBase(){}





protected slots:

    void legendClicked(QwtPlotItem * plotItem);

    void legendChecked(QwtPlotItem * plotItem, bool on);

private:

    AnalogPlot* m_pAnalogPlot;

    QTimer* m_pTimer;
};

#endif // SCOPEPLOTBASE_H
