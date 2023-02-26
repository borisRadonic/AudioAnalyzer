#include "formscope.h"
#include "ui_formscope.h"
#include "analogplot.h"
#include "analogsignal.h"
#include "mainwindow.h"
#include <QDebug>
#include <QThread>
#include <QCloseEvent>
#include <QSettings>
#include <QMutex>

FormScope::FormScope( QSettings* settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormScope),
    m_channelOneEnabled(true),
    m_channelTwoEnabled(true),
    m_timeBase(1000),
    m_connected(false),
    m_sampleTime( 1 /96000 ),
    m_triggered(false),
    m_triggerLevel(0.0),
    m_waitTrigger(false),
    m_triggerSource(0),
    m_currentSample(0),
    m_numOfSamples(0),
    m_dataChanged(false),
    m_refCounter(0),
    m_ampCh1(1.0),
    m_ampCh2(1.0),
    m_mainWindow( (MainWindow*) parent),
    m_settings( settings )
{
    ui->setupUi(this);


    m_plot = new AnalogPlot(ui->scopeWidget, "Scope" );
    m_mutex = new QMutex();
    m_mutexExchange = new QMutex();

    m_refreshTimer = new QTimer();

    m_pAnalogSignalChannel1 = new AnalogSignal();
    m_pAnalogSignalChannel2 = new AnalogSignal();

    QString ch1 = "CH1";
    QString ch2 = "CH2";

    m_plot->AddAnalogSignal( m_pAnalogSignalChannel1, QColor::fromRgb( 90,216,7), ch1 );
    m_plot->AddAnalogSignal( m_pAnalogSignalChannel2, QColor::fromRgb( 255,255,155), ch2 );

    QWidget::connect( ui->dialAmp1,                SIGNAL(  valueChanged(int)),          SLOT(amplitudeChannelOneChanged(int)));
    QWidget::connect( ui->dialAmp2,                SIGNAL(  valueChanged(int)),          SLOT(amplitudeChannelTwoChanged(int)));
    QWidget::connect( ui->doubleSpinBoxAmp1,       SIGNAL(  valueChanged(double)),       SLOT(amplitudeSpinChannelOneChanged(double)));
    QWidget::connect( ui->doubleSpinBoxAmp2,       SIGNAL(  valueChanged(double)),       SLOT(amplitudeSpinChannelTwoChanged(double)));
    QWidget::connect( ui->pushButtonCh1,           SIGNAL(  toggled(bool) ),             SLOT(channelOneEnable(bool)));
    QWidget::connect( ui->pushButtonCh2,           SIGNAL(  toggled(bool) ),             SLOT(channelTwoEnable(bool)));
    QWidget::connect( ui->dialTime,                SIGNAL(  valueChanged(int)),          SLOT(timeChanged(int)));
    QWidget::connect( ui->comboBoxTrigger,         SIGNAL(  currentIndexChanged(int)),   SLOT(triggerSourceChanged(int)));
    QWidget::connect( ui->doubleSpinBoxTriggLevel, SIGNAL(  valueChanged(double)),       SLOT(triggerLevelChanged(double)));
    QWidget::connect (m_refreshTimer,              SIGNAL(  timeout()),                  SLOT(refreshTimerExpired()));

    m_refreshTimer->start( 10  );

    setAttribute(Qt::WA_DeleteOnClose, true);

    setDefaults();

    workerThread = new ScopeWorkerThread(this);
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();

}

FormScope::~FormScope()
{
    m_refreshTimer->stop();
    delete m_refreshTimer;
    delete m_pAnalogSignalChannel1;
    delete m_pAnalogSignalChannel2;
    delete m_plot;
    delete ui;
    //delete m_mutex;
    //delete m_mutexExchange;
    //m_mutexExchange = NULL;
}

void FormScope::insertScopeData( const QVector<double>& channel1, const QVector<double>& channel2 )
{
    m_mutexExchange->lock();
    m_channel1 = channel1;
    m_channel2 = channel2;
    m_mutexExchange->unlock();
}

void FormScope::setSampleTime( double time )
{
    m_sampleTime = time;
    m_numOfSamples = ( ((double) m_timeBase / 1000000.0)  / m_sampleTime);
}

void FormScope::connectToScope()
{
    m_connected = true;
}

void FormScope::disconnectFromScope()
{
    m_connected = false;
}

void FormScope::amplitudeChannelOneChanged( int value )
{
    ui->doubleSpinBoxAmp1->setValue( (double) value * 0.1 );
    m_ampCh1 = (double) value * 0.1 ;
    m_triggered = false;
    m_dataChanged = true;
    refreshTimerExpired();
}

void FormScope::amplitudeChannelTwoChanged( int value )
{
      ui->doubleSpinBoxAmp2->setValue( value * 0.1 );
      m_ampCh2 = (double) value * 0.1 ;
      m_triggered = false;
      m_dataChanged = true;
      refreshTimerExpired();
}

void FormScope::triggerSourceChanged( int value )
{
    m_triggerSource = value;
}

void FormScope::amplitudeSpinChannelOneChanged( double value )
{
     ui->dialAmp1->setValue(value * 10);
}

void FormScope::amplitudeSpinChannelTwoChanged( double value )
{
     ui->dialAmp2->setValue(value * 10);
}

void FormScope::triggerLevelChanged( double value )
{
    m_triggered = false;
    m_dataChanged = true;
    refreshTimerExpired();
    m_triggerLevel = value;
}

void FormScope::setDefaults()
{
    int value = m_settings->value( "ScopeAmp1", 10 ).toInt();
    ui->dialAmp1->setValue(value);

    value = m_settings->value( "ScopeAmp2", 10 ).toInt();
    ui->dialAmp2->setValue(value);

    value = m_settings->value( "ScopeTime", 5 ).toInt();
    ui->dialTime->setValue( value );

    value = m_settings->value( "TriggerSource", 0 ).toInt();
    ui->comboBoxTrigger->setCurrentIndex( value );
}

void FormScope::channelOneEnable( bool enabled )
{
     qWarning() << "channelOneEnable " << enabled;
     m_channelOneEnabled = enabled;
}


void FormScope::channelTwoEnable( bool enabled )
{
     qWarning() << "channelTwoEnable " << enabled;

     m_channelTwoEnabled = enabled;
}

void FormScope::timeChanged( int value )
{
    switch( value )
    {
    case 1:
    {
        ui->labelTimeBase->setText( "1 us");
        m_timeBase = 1;
        break;
    }
    case 2:
    {
        ui->labelTimeBase->setText( "10 us");
        m_timeBase = 10;
        break;
    }
    case 3:
    {
        ui->labelTimeBase->setText( "50 us");
        m_timeBase = 50;
        break;
    }
    case 4:
    {
        ui->labelTimeBase->setText( "100 us");
        m_timeBase = 100;
        break;
    }
    case 5:
    {
        ui->labelTimeBase->setText( "1 ms");
        m_timeBase = 1000;
        break;
    }
    case 6:
    {
        ui->labelTimeBase->setText( "5 ms");
        m_timeBase = 5000;
        break;
    }
    case 7:
    {
        ui->labelTimeBase->setText( "10 ms");
        m_timeBase = 10000;
        break;
    }
    case 8:
    {
        ui->labelTimeBase->setText( "20 ms");
        m_timeBase = 20000;
        break;
    }
    case 9:
    {
        ui->labelTimeBase->setText( "50 ms");
        m_timeBase = 50000;
        break;
    } 
    }
    m_plot->setTimeBase( m_timeBase );
    m_numOfSamples = ( ((double) m_timeBase / 1000000.0)  / m_sampleTime);
    //qDebug() << "m_numOfSamples " << m_numOfSamples;
    m_triggered = false;
    m_dataChanged = true;
    refreshTimerExpired();
}

void FormScope::refreshTimerExpired()
{
    if( m_dataChanged )
    {
        //transfer data to plots
        m_pAnalogSignalChannel1->addValues( timeValues, yValuesChannel1);
        m_pAnalogSignalChannel2->addValues( timeValues, yValuesChannel2);
        m_plot->PlotSignals();
        m_mutex->lock();
        m_triggered = false;
        m_mutex->unlock();
        m_refCounter = 0;
    }
    else
    {
        if( m_refCounter > 5 )
        {
           //m_triggered = false;
           m_refCounter = 0;
        }
        m_refCounter++;
    }
}

void FormScope::closeEvent(QCloseEvent*)
{
    m_settings->setValue( "ScopeAmp1", ui->dialAmp1->value() );
    m_settings->setValue( "ScopeAmp2", ui->dialAmp2->value() );
    m_settings->setValue( "ScopeTime", ui->dialTime->value() );
    m_settings->setValue( "TriggerSource", ui->comboBoxTrigger->currentIndex() );

    while (workerThread->isRunning() )
    {
        workerThread->stop();
    }
    m_mainWindow->setFormScopeClosed();
}

void FormScope::resizeEvent(QResizeEvent*)
{
    int h = ui->gridLayout->geometry().height();
    int w = ui->gridLayout->geometry().width();
    QwtPlot * plotWidget = m_plot->GetPlotWidget();
    plotWidget->resize( w, h );
    plotWidget->replot();
}

void FormScope::processScopeData()
{
    QVector<double> channel1;
    QVector<double> channel2;
    bool data(false);

    if( !m_mutexExchange )
    {
        return;
    }
    m_mutexExchange->lock();

    if( m_channel1.length() > 0 )
    {
        data = true;
        channel1 = m_channel1;
        channel2 = m_channel1;
    }
    else
    {
        data = false;
        if( yValuesChannel1.size() > 0 )
        {
            yValuesChannel1.clear();
            yValuesChannel2.clear();
            timeValues.clear();
        }
    }

    m_mutexExchange->unlock();
    if( data )
    {
        double value(0.0);
        m_mutex->lock();
        for ( int i = 0; i < channel1.length(); i++ )
        {
            if( !m_triggered )
            {
                m_dataChanged = false;
                if( m_triggerSource == 0)
                {
                    //channel 1
                    value = channel1[i];
                }
                else
                {
                    //channel 2
                    value = channel2[i];
                }
                if( value <= m_triggerLevel )
                {
                    m_waitTrigger = true;
                    m_currentSample = 0;
                    if( yValuesChannel1.size() > 0 )
                    {
                        yValuesChannel1.clear();
                        yValuesChannel2.clear();
                        timeValues.clear();
                        yValuesChannel1.reserve( m_numOfSamples );
                        yValuesChannel2.reserve( m_numOfSamples );
                        timeValues.reserve( m_numOfSamples );

                    }
                    continue;
                }
                if( m_waitTrigger )
                {
                    if( value > m_triggerLevel )
                    {
                        m_triggered = true;
                        m_currentSample = 0;
                        m_waitTrigger = false;
                    }
                }
            }

            if( m_triggered && m_currentSample < m_numOfSamples )
            {
                double time(0.0);

                time = 10.0 * m_currentSample * m_sampleTime * 1000000.0 / (double) m_timeBase;

                yValuesChannel1.push_back( channel1[i] * m_ampCh1 );
                yValuesChannel2.push_back( channel2[i] * m_ampCh2 );
                timeValues.push_back( time );

                m_dataChanged = true;

                m_currentSample++;
            }
            if( m_currentSample > m_numOfSamples )
            {
                m_currentSample = 0;
            }
        }
        m_mutex->unlock();
    }
    else
    {
        QThread::msleep( 100 );
    }
}

void ScopeWorkerThread::run()
{
    while( m_run )
    {
        m_formScope->processScopeData();
    }
    return;
}
