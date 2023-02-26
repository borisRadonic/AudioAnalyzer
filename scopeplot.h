#ifndef COSCPLOT_H
#define COSCPLOT_H

class ScopePlotCurve;
class ScopePlotCurve;
class QwtPlotMarker;

class ScopePlot
{
public:

    ScopePlot():m_timeBase(1000)
    {
    }

    virtual ~ScopePlot(void)
    {
    }

    virtual void PlotSignals() = 0;

    virtual void PlotLine() = 0;

    virtual int getMarkerPosFromValue (double)
    {
        return -1;
    }

    virtual void getDisplayedRange (double*, double*) {}

    inline virtual void setLineValue(int x)
    {
        m_xLineValue = x;
    }

protected:

    int m_xLineValue;

    unsigned int m_timeBase;

};

#endif // COSCPLOT_H
