#ifndef FORMSIGGEN_H
#define FORMSIGGEN_H

#include <QWidget>
#include "signalgenerator.h"

namespace Ui {
class FormSigGen;
}

class RtAudio;
class AudioInterface;
class MainWindow;
class QSettings;
class QwtPlot;
class QwtPlotCurve;
class RandomNumber;

class FormSigGen : public QWidget
{
    Q_OBJECT

public:
    explicit FormSigGen( QSettings* settings, AudioInterface* audioInterace, QWidget *parent = 0);
    ~FormSigGen();

    inline void connectToSignalGenerator( RtAudio* audio, AudioInterface* audioInterace, int currentSampleRate )
    {
        m_audio                 = audio;
        m_audioInterace         = audioInterace;
        m_currentSampleRate     = currentSampleRate;
    }

    inline void disconnectromSignalGenerator()
    {
        m_audio                 = NULL;
        m_audioInterace       = NULL;
        m_currentSampleRate     = 0;
    }

    inline void getSigGenParams(  SignalGenerator::SignalType& sigType, unsigned int& freq1, unsigned int& freq2 )
    {
        sigType = m_sigType;
        freq1   = m_freq1;
        freq2   = m_freq2;
    }

protected:

    virtual void closeEvent(QCloseEvent *e);
    virtual void resizeEvent(QResizeEvent *);

private:

    void setDefaults();
    void enableAll( bool enabled );
    void showPlot();

private slots:


    void startPressed( bool start );
    void functionChanged(int index);
    void freq1Changed(int value);
    void freq2Changed(int value);
    void phi1Changed(int value);
    void phi2Changed(int value);
    void doubleSpinBoxAmp1Changed(double value);
    void doubleSpinBoxAmp2Changed(double value);
    void outChannel1Changed( int state );
    void outChannel2Changed( int state );

private:
    Ui::FormSigGen      *ui;
    RtAudio*            m_audio;
    AudioInterface*     m_audioInterace;
    MainWindow*         m_mainWindow;
    QSettings*          m_settings;
    QwtPlot*            m_plot;
    QwtPlotCurve*       m_pPlotCurve;

    int                 m_currentSampleRate;
    bool                m_out1;
    bool                m_out2;

    SignalGenerator::SignalType m_sigType;
    unsigned int                m_freq1;
    unsigned int                m_freq2;
    double                      m_phi1;
    double                      m_phi2;
    double                      m_amp1;
    double                      m_amp2;

    QVector<double> xData;
    QVector<double> yData;
    RandomNumber*   m_randomGen;

};

#endif // FORMSIGGEN_H
