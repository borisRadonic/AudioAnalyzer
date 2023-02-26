#ifndef FORMSCOPE_H
#define FORMSCOPE_H

#include <QWidget>
#include "iscope.h"
#include <QVector>
#include <QThread>
#include <QTimer>

namespace Ui
{
class FormScope;
}

class AnalogSignal;
class AnalogPlot;
class QMutex;
class QTimer;
class MainWindow;
class QSettings;
class FormScope;


class ScopeWorkerThread : public QThread
{
    Q_OBJECT

public:

    ScopeWorkerThread( FormScope* formScope )
        :m_formScope(formScope),m_run(true)
    {

    }

    void run();

    void stop()
    {
        m_run = false;
    }

protected:

    FormScope* m_formScope;
    bool m_run;
};



class FormScope : public QWidget, IScope
{
    Q_OBJECT


public:

    explicit FormScope( QSettings* settings, QWidget *parent = 0 );

    ~FormScope();

    virtual void insertScopeData( const QVector<double>& channel1, const QVector<double>& channel2 );

    virtual void setSampleTime( double time );

    virtual void connectToScope();

    virtual void disconnectFromScope();

    void processScopeData();


protected:

    virtual void closeEvent(QCloseEvent *e);
    virtual void resizeEvent(QResizeEvent *);

protected slots:

    void amplitudeChannelOneChanged( int value );
    void amplitudeChannelTwoChanged( int value );
    void triggerSourceChanged( int value );
    void amplitudeSpinChannelOneChanged( double value );
    void amplitudeSpinChannelTwoChanged( double value );
    void triggerLevelChanged( double value );
    void channelOneEnable( bool enabled );
    void channelTwoEnable( bool enabled );
    void timeChanged( int value );
    void refreshTimerExpired();


private:

    void setDefaults();

private:

    ScopeWorkerThread * workerThread;

    Ui::FormScope *ui;

    AnalogPlot* m_plot;
    QMutex* m_mutex;
    QTimer* m_refreshTimer;

    bool m_channelOneEnabled;
    bool m_channelTwoEnabled;
    int m_timeBase; //in uS
    bool m_connected;
    double m_sampleTime;
    bool m_triggered;
    double m_triggerLevel;
    bool m_waitTrigger;
    unsigned int m_triggerSource;
    unsigned int m_currentSample;
    unsigned int m_numOfSamples;
    bool m_dataChanged;
    unsigned int m_refCounter;

    double m_ampCh1;
    double m_ampCh2;

    QMutex* m_mutexExchange;
    QVector<double> m_channel1;
    QVector<double> m_channel2;

    AnalogSignal* m_pAnalogSignalChannel1;
    AnalogSignal* m_pAnalogSignalChannel2;



    QVector<double> timeValues;
    QVector<double> yValuesChannel1;
    QVector<double> yValuesChannel2;

    MainWindow* m_mainWindow;

    QSettings* m_settings;

};

#endif // FORMSCOPE_H
