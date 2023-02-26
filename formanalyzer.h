#ifndef FORMANALYZER_H
#define FORMANALYZER_H


#include "window.h"
#include "IAnalyzer.h"
#include "FrequencyPlot.h"
#include "signalgenerator.h"

#include <QWidget>
#include <QVector>
#include <QThread>

namespace Ui {
class FormAnalyzer;
}

class QSettings;
class MainWindow;
class QMutex;
class QTimer;
class FreqValues;
class AnalyzerWorkerThread;
class WeightingFilter;


class FormAnalyzer : public QWidget, IAnalyzer
{
    Q_OBJECT

public:

    explicit FormAnalyzer( QSettings* settings, QWidget *parent = 0);

    ~FormAnalyzer();

    virtual void insertAnalyzerData( const QVector<double>& channel1, const QVector<double>& channel2 );

    inline void setSamplingRate( unsigned int samplingRate )
    {
        m_samplingRate = samplingRate;
    }

    void processAnalyzerData();

    inline void setSigGenParameters( SignalGenerator::SignalType sigType, unsigned int sigGenFreq1, unsigned int sigGenFreq2 )
    {
        m_sigType       = sigType;
        m_sigGenFreq1   = sigGenFreq1;
        m_sigGenFreq2   = sigGenFreq2;
    }

     inline void resetSigGenParameters()
     {
         m_sigType       = SignalGenerator::eEnd;
         m_sigGenFreq1   = 0;
         m_sigGenFreq2   = 0;
     }

    enum Scale
    {
        Logarithmic = 0,
        Linear      = 1
    };

    enum DistorsionType
    {
        Harmonic        = 0,
        Intermodulation = 1
    };

    enum WeightingFilterType
    {
        noFilter        = 0,
        ituR468         = 1
    };

private slots:

    void channelChanged(int index);
    void fStartChanged(int value);
    void fStopChanged(int value);
    void windowLenChanged(int value);
    void scaleChanged(int index);
    void windowChanged(int index);
    void attenuationChanged(int value);
    void distorsionTypeChanged(int index);
    void weightingFilterTypeChanged(int index);
    void numHarmonicsChanged(int value);
    void fftPointsChanged( int index );
    void triggerLevelChanged( double value );
    void refreshTimerExpired();

protected:

    virtual void closeEvent(QCloseEvent *e);
    virtual void resizeEvent(QResizeEvent *);

    void readConfig();

    int fillNumFFTPointsCombo( int minPoints );

    void recomputeAll();

    void computeTimesAndRefresh();

    void showHideDistMeasurementWidgets();

private:

    Ui::FormAnalyzer *ui;
    QSettings* m_settings;
    MainWindow* m_mainWindow;
    QMutex* m_mutex;
    QTimer* m_refreshTimer;
    FrequencyPlot* m_plot;

    FreqValues* m_pFreqValues;

    unsigned int        m_channel;
    unsigned int        m_fStart;
    unsigned int        m_fStop;
    unsigned int        m_windowLen; //in Hz
    Scale               m_scale;
    BaseWindow::Window  m_window;
    unsigned int        m_attenuation;
    unsigned int        m_numHarmonics;
    unsigned int        m_numFFTPoints;
    DistorsionType      m_distorsionType;
    WeightingFilterType m_weightingFilter;
    unsigned int        m_samplingRate;
    double              m_triggerLevel;
    bool                m_triggered;
    bool                m_waitTrigger;
    unsigned int        m_currentSample;
    bool                m_dataChanged;
    unsigned int        m_refreshEveryms;
    BaseWindow*         m_pWindow;
    double*             m_fftValues;
    double*             m_valuesBuffer;
    double*             m_valNoWindowBuffer;

    QMutex*             m_mutexExchange;
    QVector<double>     m_channel1;
    QVector<double>     m_channel2;

    double*             m_averagesBuffer;
    unsigned int        m_currentAverage;
    unsigned int        m_numberOfAverages;

    AnalyzerWorkerThread * workerThread;

    SignalGenerator::SignalType m_sigType;
    unsigned int                m_sigGenFreq1;
    unsigned int                m_sigGenFreq2;

    QString                     m_thdGoertzel;
    WeightingFilter*            m_pWeightingFilter;

    QString                     m_Imd3;
    double                      m_baseF1;
    double                      m_baseF2;


};


class AnalyzerWorkerThread : public QThread
{
    Q_OBJECT

public:

    AnalyzerWorkerThread( FormAnalyzer* formAnalyzer )
        :m_formAnalyzer(formAnalyzer),m_run(true)
    {
    }

    void run();

    inline void stop()
    {
        m_run = false;
    }


protected:

    FormAnalyzer* m_formAnalyzer;
    bool          m_run;
};


#endif // FORMANALYZER_H
