#include "formanalyzer.h"
#include "ui_formanalyzer.h"
#include "mainwindow.h"
#include "four.h"
#include "weightingfilter.h"
#include <QTimer>
#include <QSettings>
#include <QDebug>
#include <QMutex>
#include "math.h"


#define DEFAULT_CHANNEL     1
#define DEFAULT_FSTART      20
#define DEFAULT_FSTOP       20000
#define DEFAULT_WINDOWLEN   20
#define DEFAULT_SCALE       FormAnalyzer::Logarithmic
#define DEFAULT_WINDOW      BaseWindow::Hann
#define DEFAULT_ATTEN       45
#define DEFAULT_FFT_POINTS  1024
#define DEFAULT_NUM_HARM    10
#define DEFAULT_DISTOR_TYPE FormAnalyzer::Harmonic
#define DEFAULT_TRIG_LEVEL  0.0001
#define DEFAULT_WF          FormAnalyzer::noFilter

#define MAX_FFT_POINTS  192000

#define VALUES_BUFFER_SIZE sizeof(double) * MAX_FFT_POINTS * 2
#define MAX_AVARAGES 5
#define MAX_HARMONICS 10


FormAnalyzer::FormAnalyzer( QSettings* settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAnalyzer),
    m_settings( settings ),
    m_mainWindow( (MainWindow*) parent),
    m_channel(DEFAULT_CHANNEL),
    m_fStart(DEFAULT_FSTART),
    m_fStop(DEFAULT_FSTOP),
    m_windowLen(DEFAULT_WINDOWLEN),
    m_scale( DEFAULT_SCALE ),
    m_window( DEFAULT_WINDOW ),
    m_attenuation( DEFAULT_ATTEN ),
    m_numHarmonics( DEFAULT_NUM_HARM ),
    m_numFFTPoints( DEFAULT_FFT_POINTS ),
    m_distorsionType( DEFAULT_DISTOR_TYPE ),
    m_weightingFilter(DEFAULT_WF),
    m_samplingRate(96000),
    m_triggerLevel(DEFAULT_TRIG_LEVEL),
    m_triggered(false),
    m_waitTrigger(false),
    m_currentSample(0),
    m_dataChanged(false),
    m_refreshEveryms(1000),
    m_currentAverage(0),
    m_numberOfAverages(MAX_AVARAGES),
    m_sigType( SignalGenerator::eEnd ),
    m_sigGenFreq1(0),
    m_sigGenFreq2(0),
    m_pWeightingFilter( NULL ),
    m_baseF1(0.0),
    m_baseF2(0.0)
{
    ui->setupUi(this);

    m_fftValues = NULL;

    m_mutex = new QMutex();
    m_mutexExchange = new QMutex();
    m_refreshTimer = new QTimer();

    m_valuesBuffer      = (double*) malloc( VALUES_BUFFER_SIZE );
    m_valNoWindowBuffer = (double*) malloc( VALUES_BUFFER_SIZE );
    m_fftValues         = (double*) malloc( VALUES_BUFFER_SIZE );
    m_averagesBuffer    = (double*) malloc( VALUES_BUFFER_SIZE * MAX_AVARAGES );

    m_plot = new FrequencyPlot( ui->widgetAnalyzer, "Spectrum Analyzer" );
    m_mutex = new QMutex();
    m_refreshTimer = new QTimer();

    m_pFreqValues = new FreqValues();
    QString f("FFT");

    m_plot->AddFreqValue( m_pFreqValues, QColor::fromRgb( 90,216,7), f );


    m_pWindow = WindowFactory::create( DEFAULT_WINDOW, m_samplingRate/ m_windowLen );

    ui->comboBoxWindow->insertItem( (int) BaseWindow::Rectangular, "Rectangular" );

    ui->comboBoxWindow->insertItem( (int) BaseWindow::FlatTop, "FlatTop" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Hamming, "Hamming" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Hann, "Hann" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::BartlettHann, "BartlettHann" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Blackman, "Blackman" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::BlackmanHarris, "BlackmanHarris" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Nuttall, "Nuttall" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::BlackmanNuttall, "BlackmanNuttall" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Parzen, "Parzen" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Triangular, "Triangular" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Tukey, "Tukey" );
    ui->comboBoxWindow->insertItem( (int) BaseWindow::Welch, "Welch" );
    //ui->comboBoxWindow->insertItem( (int) BaseWindow::Lanczos, "Lanczos" );
     //ui->comboBoxWindow->insertItem( (int) BaseWindow::Kaiser, "Kaiser" );
     //ui->comboBoxWindow->insertItem( (int) BaseWindow::Chebyshev, "Chebyshev" );

    connect( ui->comboBoxChannel,           SIGNAL(  currentIndexChanged(int)),     SLOT(channelChanged(int)));
    connect( ui->spinBoxFStart,             SIGNAL(  valueChanged(int)),            SLOT(fStartChanged(int)));
    connect( ui->spinBoxFStop,              SIGNAL(  valueChanged(int)),            SLOT(fStopChanged(int)));
    connect( ui->spinBoxWindowLen,          SIGNAL(  valueChanged(int)),            SLOT(windowLenChanged(int)));
    connect( ui->comboBoxScale,             SIGNAL(  currentIndexChanged(int)),     SLOT(scaleChanged(int)));
    connect( ui->comboBoxWindow,            SIGNAL(  currentIndexChanged(int)),     SLOT(windowChanged(int)));
    //connect( ui->spinBoxAttenuation,      SIGNAL(  valueChanged(int)),            SLOT(attenuationChanged(int)));
    connect( ui->comboBoxDistorsionType,    SIGNAL(  currentIndexChanged(int)),     SLOT(distorsionTypeChanged(int)));
    connect( ui->comboBoxWeightingFilter,   SIGNAL(  currentIndexChanged(int)),     SLOT(weightingFilterTypeChanged(int)));
    connect( ui->spinBoxNumHarmonics,       SIGNAL(  valueChanged(int)),            SLOT(numHarmonicsChanged(int)));
    connect( ui->comboBoxFFTPoints,         SIGNAL(  currentIndexChanged(int)),     SLOT(fftPointsChanged(int)));
    connect( ui->spinBoxTriggerLevel,       SIGNAL(  valueChanged(double)),         SLOT(triggerLevelChanged(double)));


    connect (m_refreshTimer,                SIGNAL(  timeout()),                    SLOT(refreshTimerExpired()));

    ui->labelAtt->setVisible( false );
    ui->spinBoxAttenuation->setVisible(false);
    fillNumFFTPointsCombo( 1024 );

    readConfig();

    computeTimesAndRefresh();

    QTableWidgetItem* item = NULL;
    ui->tableWidgetHarmonics->setRowCount(10);
    ui->tableWidgetHarmonics->setColumnCount(2);
    ui->tableWidgetHarmonics->setColumnWidth(0,80);
    ui->tableWidgetHarmonics->setColumnWidth(1,80);
    for( int i = 0; i < MAX_HARMONICS; i++ )
    {
        item = new QTableWidgetItem( "0");
        ui->tableWidgetHarmonics->setItem( i, 0,item  );

        item = new QTableWidgetItem( "0.0");
        ui->tableWidgetHarmonics->setItem( i, 1,item  );
    }

    ui->labelValueTHD->setText("");

    m_dataChanged = false;

    workerThread = new AnalyzerWorkerThread(this);
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
}

FormAnalyzer::~FormAnalyzer()
{
    delete ui;
    delete m_plot;
    delete m_pFreqValues;
    delete m_mutexExchange;
    delete m_mutex;
    delete m_refreshTimer;
    delete m_pWindow;
    delete m_valuesBuffer;
    delete m_valNoWindowBuffer;
    delete m_fftValues;
    delete m_averagesBuffer;
    delete workerThread;

}

void FormAnalyzer::insertAnalyzerData( const QVector<double>& channel1, const QVector<double>& channel2 )
{
    m_mutexExchange->lock();
    m_channel1 = channel1;
    m_channel2 = channel2;
    m_mutexExchange->unlock();
}

void FormAnalyzer::readConfig()
{
    int value = m_settings->value( "SpecChannel", DEFAULT_CHANNEL ).toInt();
    ui->comboBoxChannel->setCurrentIndex(value);

    value = m_settings->value( "SpecFStart", DEFAULT_FSTART ).toInt();
    ui->spinBoxFStart->setValue(value);

    value = m_settings->value( "SpecFStop", DEFAULT_FSTOP ).toInt();
    ui->spinBoxFStop->setValue(value);

    value = m_settings->value( "SpecWinLen", DEFAULT_WINDOWLEN ).toInt();
    ui->spinBoxWindowLen->setValue( value );

    value = m_settings->value( "SpecScale", DEFAULT_SCALE ).toInt();
    ui->comboBoxScale->setCurrentIndex( value );

    value = m_settings->value( "SpecWindow", DEFAULT_WINDOW ).toInt();
    ui->comboBoxWindow->setCurrentIndex( value );

    value = m_settings->value( "SpecAtten", DEFAULT_ATTEN ).toInt();
    ui->spinBoxAttenuation->setValue( value );

    value = m_settings->value( "SpecNumFFT", DEFAULT_FFT_POINTS ).toInt();
    //ui->spinBoxOverlap->setValue( value );

    value = m_settings->value( "SpecNumHarm", DEFAULT_NUM_HARM ).toInt();
    ui->spinBoxNumHarmonics->setValue( value );

    value = m_settings->value( "SpecNumDistType", DEFAULT_DISTOR_TYPE ).toInt();
    ui->comboBoxDistorsionType->setCurrentIndex( value );

    value = m_settings->value( "SpecNumWF", DEFAULT_WF ).toInt();
    ui->comboBoxWeightingFilter->setCurrentIndex( value );

    m_triggerLevel =  m_settings->value( "SpecTrigLevel", DEFAULT_TRIG_LEVEL ).toDouble();
    ui->spinBoxTriggerLevel->setValue( m_triggerLevel);
}

void FormAnalyzer::channelChanged(int index)
{
    m_channel = index;
}

void FormAnalyzer::fStartChanged(int value)
{
    m_fStart = value;
    recomputeAll();
    m_plot->changeFreqRange( m_fStart, m_fStop );
}

void FormAnalyzer::fStopChanged(int value)
{
    m_fStop = value;
    recomputeAll();
    m_plot->changeFreqRange( m_fStart, m_fStop );
}

void FormAnalyzer::windowLenChanged(int value)
{
    m_windowLen = value;
    //change the number of FFT points in ComboBox to select
    int minPoints = m_samplingRate / m_windowLen;
    if( m_sigType == SignalGenerator::eSinus/* || m_sigType == SignalGenerator::eSinPlusSin*/)
    {
        if( (unsigned int )minPoints < ( m_samplingRate / m_sigGenFreq1 ) )
        {
            minPoints =  m_samplingRate / m_sigGenFreq1;
        }
    }

    qDebug() << "minPoints = " << minPoints;
    m_numFFTPoints = fillNumFFTPointsCombo( minPoints );
    recomputeAll();
    computeTimesAndRefresh();
}

void FormAnalyzer::scaleChanged(int index)
{
    m_scale = (Scale) index;
}

void FormAnalyzer::windowChanged(int index)
{
    m_window = (BaseWindow::Window) index;
    recomputeAll();
}

void FormAnalyzer::attenuationChanged(int value)
{
    m_attenuation = value;
    recomputeAll();
}

void FormAnalyzer::distorsionTypeChanged(int index)
{
    m_distorsionType = (DistorsionType) index;
    showHideDistMeasurementWidgets();
}

void FormAnalyzer::weightingFilterTypeChanged(int index)
{
    if(  m_weightingFilter != (WeightingFilterType) index )
    {
        m_weightingFilter = (WeightingFilterType) index;
        if( m_pWeightingFilter)
        {
            delete m_pWeightingFilter;
        }
        if ( FormAnalyzer::noFilter == m_weightingFilter )
        {
            m_pWeightingFilter = new WeightingFilterNoFilter();
        }
        else if ( FormAnalyzer::ituR468 == m_weightingFilter )
        {
             m_pWeightingFilter = new WeightingFilterItuR468();
        }
        else
        {
            m_pWeightingFilter = NULL;
        }
    }
}

void FormAnalyzer::numHarmonicsChanged(int value)
{
    for ( int i = 0; i < ui->tableWidgetHarmonics->rowCount(); i++ )
    {
        ui->tableWidgetHarmonics->item(i,0)->setText("");
        ui->tableWidgetHarmonics->item(i,1)->setText("");
    }
    m_numHarmonics = value;
}

void FormAnalyzer::fftPointsChanged( int index )
{
    m_numFFTPoints = 1 << (10+index);
    recomputeAll();
    computeTimesAndRefresh();
}

void FormAnalyzer::triggerLevelChanged( double value )
{
    m_triggered = false;
    m_dataChanged = true;
    refreshTimerExpired();
    m_triggerLevel = value;
    qDebug() << "Trigger level =" << value;
}

void FormAnalyzer::recomputeAll()
{
    double omega(0.0);

    m_mutex->lock();

    if( m_pWindow )
    {
        delete m_pWindow;
    }

    m_pWindow = WindowFactory::create( m_window, m_numFFTPoints, m_attenuation );

    int numOfCoefs = m_pWindow->length();
    double nominator(0.0), denominator(0.0);

    for( int i = 0; i < numOfCoefs; i++)
    {
        omega = m_pWindow->w(i);
        nominator += omega * omega;
        denominator += omega;
    }
    double normEffNoiseBandwith(1.0); //The normalized effective noise bandwidt
    if( denominator > 0 )
    {
        normEffNoiseBandwith = numOfCoefs * nominator/ ( denominator * denominator );
    }

     m_mutex->unlock();
    //at the moment only info
    //
    ui->labelValueNENBW->setText( QString::number( normEffNoiseBandwith, 'f', 6 ) );

}

void FormAnalyzer::closeEvent(QCloseEvent* /*e*/)
{
    m_settings->setValue( "SpecChannel",        ui->comboBoxChannel->currentIndex()         );
    m_settings->setValue( "SpecFStart",         ui->spinBoxFStart->value()                  );
    m_settings->setValue( "SpecFStop",          ui->spinBoxFStop->value()                   );
    m_settings->setValue( "SpecWinLen",         ui->spinBoxWindowLen->value()               );
    m_settings->setValue( "SpecScale",          ui->comboBoxScale->currentIndex()           );
    m_settings->setValue( "SpecWindow",         ui->comboBoxWindow->currentIndex()          );
    m_settings->setValue( "SpecAtten",          ui->spinBoxAttenuation->value()             );
    m_settings->setValue( "SpecNumFFT",         m_numFFTPoints                              );
    m_settings->setValue( "SpecNumHarm",        ui->spinBoxNumHarmonics->value()            );
    m_settings->setValue( "SpecNumDistType",    ui->comboBoxDistorsionType->currentIndex()  );
    m_settings->setValue( "SpecNumWF",          ui->comboBoxWeightingFilter->currentIndex()  );
    m_settings->setValue( "SpecTrigLevel",      ui->spinBoxTriggerLevel->value()            );

    while (workerThread->isRunning() )
    {
        workerThread->stop();
    }

    m_mainWindow->setFormAnalyzerClosed();
}

int FormAnalyzer::fillNumFFTPointsCombo( int minPoints )
{
     int last(1024);
     int ret(0);
     ui->comboBoxFFTPoints->clear();

     if( minPoints <= 1024 )
     {
         ui->comboBoxFFTPoints->insertItem( 0, "1024" );
     }
     else if( minPoints > 1024 && minPoints <= 2048 )
     {
         ui->comboBoxFFTPoints->insertItem( 1, "2048" );
         last = 2048;
     }
     else if( minPoints > 2048 && minPoints <= 4096 )
     {
         ui->comboBoxFFTPoints->insertItem( 2, "4096" );
         last = 4096;
     }
     else if( minPoints > 4096 && minPoints <= 8192 )
     {
         ui->comboBoxFFTPoints->insertItem( 3, "8192" );
         last = 8192;
     }
     else if( minPoints > 8192 && minPoints <= 16384 )
     {
         ui->comboBoxFFTPoints->insertItem( 4, "16384" );
         last = 16384;
     }
     else if( minPoints > 16384 && minPoints <= 32768 )
     {
         ui->comboBoxFFTPoints->insertItem( 5, "32768" );
         last = 32768;
     }
     else if( minPoints > 32768 && minPoints <= 65536 )
     {
         ui->comboBoxFFTPoints->insertItem( 6, "32768" );
         last = 32768;
     }
     ret = last;
     int i = last;
     while( i < 65536 )
     {
         i = i * 2;
         ui->comboBoxFFTPoints->insertItem( i, QString::number(i) );
     }
     return ret;
}

void FormAnalyzer::computeTimesAndRefresh()
{
    //determine number of averages based on current settings
    //refresh rate is 1 or 2 Hz
    m_mutex->lock();
    int measurementsPerSecond = m_samplingRate / m_numFFTPoints;
    if( measurementsPerSecond < 2 )
    {
        m_refreshEveryms = 1000;
    }
    else if( measurementsPerSecond >=2 )
    {
        m_refreshEveryms = 500;
    }

    m_triggered = false;
    m_waitTrigger = true;

    if( m_fftValues )
    {
        memset( m_fftValues, 0, VALUES_BUFFER_SIZE  );
    }

    m_refreshTimer->start( m_refreshEveryms );

    m_mutex->unlock();
}

void FormAnalyzer::refreshTimerExpired()
{
    if( m_dataChanged )
    {
        QVector<double> xValues;
        QVector<double> yValues;
        QVector<double> magValues;
        m_mutex->lock();
        unsigned int index(0);
        double average(0.0);
        double averageMag(0.0);
        double val(0.0);
        double mag(0.0);
        for( unsigned int k = 0; k < m_numFFTPoints/2 - 1; k++ )
        {
            for( unsigned int i = 0; i < m_numberOfAverages; i++ )
            {
                index = i * MAX_FFT_POINTS;
                val = FFTUtility::fftMagdB( (m_averagesBuffer+index), m_numFFTPoints, k, 1.0 );
                mag = FFTUtility::fftMagnitude( (m_averagesBuffer+index), m_numFFTPoints, k );
                average     +=  val;
                averageMag  +=  mag;
            }
            average = average / (double) m_numberOfAverages;
            averageMag = averageMag / (double) m_numberOfAverages;
            double freq =  FFTUtility::fftFrequency (m_numFFTPoints, k, (double )m_samplingRate);
            xValues.push_back( freq );
            yValues.push_back( average );
            magValues.push_back( averageMag );
        }
        m_triggered = false;
        m_mutex->unlock();
        //transfer data to plots
        m_pFreqValues->addValues( xValues, yValues);
        m_plot->Plot();

        unsigned int baseFreq(0);
        double freq(0.0);
        double maxBaseValue(0.0);
        double maxFreqBase(0.0);
        double maxBaseDb(0.0);
        for( int i = 0; i < magValues.size(); i++ )
        {
            double val = magValues.at(i);
            if( m_pWeightingFilter )
            {
                 val = val * m_pWeightingFilter->getMagnitude( xValues.at(i) );
            }
            if( val > maxBaseValue )
            {

                maxBaseValue = val;

                maxBaseDb = yValues.at(i);
                maxFreqBase = xValues.at(i);
            }
        }

        double totalNoise(0.0);
        //calculate total noise
        for( int i = 0; i < xValues.size(); i++ )
        {
            double valN = magValues.at(i);
            if( m_pWeightingFilter && fabs(valN) > 0.0 )
            {
                valN = valN * m_pWeightingFilter->getMagnitude( xValues.at(i) );
            }
            totalNoise += (valN * valN);
        }

        showHideDistMeasurementWidgets();

        if( m_sigType == SignalGenerator::eSNTest )
        {
            //display SNR
            double noise = 20.0*log10( sqrt(totalNoise));
            char temp[100];
            sprintf( temp, "%.2f dB", noise );
            QString noiseText = temp;
            ui->labelValueSNR->setText( noiseText );
            m_plot->showPeak( 0, 0, 0, 0 );
        }

        if(  m_sigGenFreq1 > 0 && (m_sigType == SignalGenerator::eSinus ) && ( m_sigType != SignalGenerator::eSNTest) )
        {
             m_plot->showPeak( maxFreqBase, maxBaseDb, 0, 0 );
             baseFreq = m_sigGenFreq1;
             ui->labelValueTHDG->setText( m_thdGoertzel );
        }
        else if(  m_sigGenFreq1 > 0 && m_sigGenFreq2 > 0 && (m_sigType == SignalGenerator::eSinPlusSin) )
        {
            double val1(0.0);
            double val2(0.0);
            unsigned int index = (unsigned int) ((double) ( (double) m_numFFTPoints *  m_sigGenFreq1  ) / (double ) m_samplingRate);
            if( index <= (m_numFFTPoints/2 - 1) )
            {
                val1     = yValues.at(index);
                if( m_pWeightingFilter )
                {
                    val1 = val1 + m_pWeightingFilter->getMagnitudeIndB( xValues.at(index) );
                }
            }
            index = (unsigned int) ((double) ( (double) m_numFFTPoints *  m_sigGenFreq2 ) / (double ) m_samplingRate);
            if( index <= (m_numFFTPoints/2 - 1) )
            {
                val2     = yValues.at(index);
                if( m_pWeightingFilter )
                {
                    val2 = val2 + m_pWeightingFilter->getMagnitudeIndB( xValues.at(index) );
                }
            }

            m_plot->showPeak( m_sigGenFreq1, val1, m_sigGenFreq2, val2 );
            ui->labelValueTHD->setText(m_Imd3);
        }
        else if ( m_sigType != SignalGenerator::eSNTest && m_sigType != SignalGenerator::eSinPlusSin)
        {
            m_plot->showPeak( maxFreqBase, maxBaseDb, 0, 0 );
            //try to find the most dominant frequency
            double max(-1000.00);

            for( int i = 0; i < xValues.size(); i++ )
            {
                if( yValues.at(i) > max )
                {
                    max = yValues.at(i);
                    freq = xValues.at(i);
                }
            }
            //round it to nearest regarding m_windowLen and frequency
            unsigned int roundLen(1);
            if(  m_windowLen < 100 )
            {
                if( freq < 100 )
                {
                    roundLen = 10;
                }
                else if( freq > 100 )
                {
                    roundLen = 100;
                }
            }
            else
            {
                roundLen = 100;
            }
            baseFreq = (unsigned int) ( freq / (double) roundLen) * roundLen; //base frequency
            //qDebug() << "BaseFreq = " << baseFreq << " Mag = " << max;
        }

        if( m_sigType != SignalGenerator::eSinPlusSin && m_sigType != SignalGenerator::eSNTest )
        {
            double uSquare(0.0);
            for( unsigned int i = 2; i <= m_numHarmonics+1; i++)
            {
                unsigned int index = (unsigned int) ((double) ( (double) m_numFFTPoints *  baseFreq * (double) i) / (double ) m_samplingRate);
                if( index <= (m_numFFTPoints/2 - 1) )
                {
                    val     = yValues.at(index);
                    if( m_pWeightingFilter )
                    {
                        val = val + m_pWeightingFilter->getMagnitudeIndB( xValues.at(index) );
                    }
                    freq    = xValues.at(index);
                    mag     = magValues.at(index);

                    if( m_pWeightingFilter )
                    {
                        mag = mag * m_pWeightingFilter->getMagnitude( xValues.at(index) );
                    }

                    QTableWidgetItem* item = ui->tableWidgetHarmonics->item(i-2,0);
                    if( item )
                    {
                        item->setText(QString::number(freq));
                    }
                    item = ui->tableWidgetHarmonics->item(i-2,1);
                    if( item )
                    {
                        item->setText(QString::number(val));
                    }
                    uSquare += (mag*mag);
                }
            }
            if( maxBaseValue > 0 && m_sigType != SignalGenerator::eSNTest )
            {
                double thdProc =  (sqrt(uSquare)  / maxBaseValue) * 100.0;
                double thdDb = 20.0*log10( sqrt(uSquare) /maxBaseValue);

                char temp[100];
                sprintf( temp, "%.2f dB", thdDb );
                QString thdText = temp;
                thdText +=  + " (";
                sprintf( temp, "%.5f", thdProc );
                thdText += + temp;
                thdText += "%)";

                ui->labelValueTHD->setText(thdText);
            }
            else
            {
                ui->labelValueTHD->setText("");
            }
        }
    }
}

void FormAnalyzer::resizeEvent(QResizeEvent*)
{
    int h = ui->horizontalLayout->geometry().height();
    int w = ui->horizontalLayout->geometry().width() - ui->tabAnalyzer->geometry().width();
    QwtPlot * plotWidget = m_plot->GetPlotWidget();
    plotWidget->resize( w, h );
    plotWidget->replot();
}

 void FormAnalyzer::processAnalyzerData()
 {
    QVector<double> channel1;
    QVector<double> channel2;
    bool data(false);


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
    }
    m_mutexExchange->unlock();
    if( data )
    {
        double value(0.0);
        m_mutex->lock();
        for ( int i = 0; i < channel1.length(); i++ )
        {
            if( m_channel == 1 )
            {
                //channel 1
                value = channel1[i];
            }
            else
            {
                //channel 2
                value = channel2[i];
            }

            if( !m_triggered )
            {
                m_dataChanged = false;

                if( value <= m_triggerLevel )
                {
                    m_waitTrigger = true;
                    m_currentSample = 0;
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

            if( m_triggered && m_currentSample < m_numFFTPoints )
            {
                m_valuesBuffer[m_currentSample*2] = value * m_pWindow->w(m_currentSample);
                m_valuesBuffer[m_currentSample*2+1] = 0;//imaginary part
                m_valNoWindowBuffer[m_currentSample*2] = value;
                m_valNoWindowBuffer[m_currentSample*2+1] = 0;//imaginary part
                m_currentSample++;
            }


            if( m_triggered && m_currentSample >= m_numFFTPoints )
            {
                if( m_sigType != SignalGenerator::eSinPlusSin && m_distorsionType == FormAnalyzer::Harmonic )
                {
                    double numPointsPerCycle = (double)m_samplingRate / (double)m_sigGenFreq1;
                    unsigned int iCycles = (unsigned int) ((double)m_numFFTPoints/numPointsPerCycle);
                    int iNumGroetzl = iCycles * numPointsPerCycle;
                    if(  m_sigGenFreq1 > 0 && (m_sigType == SignalGenerator::eSinus ) )
                    {
                        //use Goerzel algorithmus for THD measurement
                        double base = FFTUtility::goertzel(iNumGroetzl, m_sigGenFreq1, m_samplingRate, m_valNoWindowBuffer );
                        //qDebug() << "Freq = " << (m_sigGenFreq1) << " val = " << base;
                        double thd(0.0);
                        for( unsigned int i = 2; i <= m_numHarmonics+1; i++)
                        {
                            value = FFTUtility::goertzel(iNumGroetzl, m_sigGenFreq1 * i, m_samplingRate, m_valNoWindowBuffer );
                            thd += value;
                            //qDebug() << "Freq = " << (m_sigGenFreq1 * i) << " val = " << value;
                        }
                        thd = sqrt(thd) / sqrt(base);
                        double thdDb = 20.0*log10( thd );
                        thd = thd * 100;
                        char temp[100];
                        sprintf( temp, "%.2f dB", thdDb );
                        QString t1 = temp;
                        t1 +=  + " (";
                        sprintf( temp, "%.5f", thd );
                        m_thdGoertzel = t1 + temp;
                        m_thdGoertzel += "%)";
                    }
                }
                else if( m_sigType == SignalGenerator::eSinPlusSin && m_distorsionType == FormAnalyzer::Intermodulation )
                {
                    if(  m_sigGenFreq1 > 0 && m_sigGenFreq2 > 0 && (m_sigType == SignalGenerator::eSinPlusSin ) )
                    {
                        //measure IMD

                        unsigned int freq1 = qMin( m_sigGenFreq1, m_sigGenFreq2 );
                        unsigned int freq2 = qMax( m_sigGenFreq1, m_sigGenFreq2 );

                        //use Goerzel algorithmus for IMD measurement
                        m_baseF1 = FFTUtility::goertzel( m_numFFTPoints, freq1, m_samplingRate, m_valNoWindowBuffer );
                        m_baseF2 = FFTUtility::goertzel( m_numFFTPoints, freq2, m_samplingRate, m_valNoWindowBuffer );

                        //F1 — Lower fundamental first-order frequency
                        //F2 — Upper fundamental first-order frequency
                        //2F1 - F2 — Lower intermodulation product from third-order harmonics
                        //2F2 - F1 — Upper intermodulation product from third-order harmonics

                        unsigned int f_2f1mf2 = abs( 2* freq1 - freq2 );
                        unsigned int f_2f2mf1 = abs( 2* freq2 - freq1 );

                        double u_2f1mf2 = FFTUtility::goertzel( m_numFFTPoints, f_2f1mf2, m_samplingRate, m_valNoWindowBuffer );
                        double u_2f2mf1 = FFTUtility::goertzel( m_numFFTPoints, f_2f2mf1, m_samplingRate, m_valNoWindowBuffer );

                        double valIMD = 100 * (sqrt( u_2f1mf2 * u_2f1mf2 + u_2f2mf1 * u_2f2mf1 )  / sqrt( m_baseF1 * m_baseF1  + m_baseF2 * m_baseF2  ));
                        char temp[100];
                        sprintf( temp, "%.5f", valIMD );
                        m_Imd3 = temp;
                        m_Imd3 += "%";
                    }
                }

                //do FFT
                FFTUtility::fft( m_valuesBuffer, m_numFFTPoints );
                m_currentSample = 0;

                //averages
                if( m_currentAverage < m_numberOfAverages )
                {
                    unsigned int index(0);
                    for( unsigned int n = 0; n < m_numFFTPoints*2; n++ )
                    {
                        index = m_currentAverage * MAX_FFT_POINTS + n;
                        m_averagesBuffer[index]= m_valuesBuffer[n];
                    }

                    memset( m_valuesBuffer, 0, VALUES_BUFFER_SIZE );
                    memset( m_valNoWindowBuffer, 0, VALUES_BUFFER_SIZE );
                    m_currentAverage++;

                    continue;
                }
                m_currentAverage = 0;
                m_dataChanged = true;
                continue;
            }
        }
        m_mutex->unlock();
    }
    else
    {
        QThread::msleep( 100 );
    }
}

void FormAnalyzer::showHideDistMeasurementWidgets()
{
    if ( m_distorsionType == FormAnalyzer::Harmonic )
    {
        ui->labelTHD1->setText("THD:");
        ui->labelTHD2->setVisible( true );
        ui->labelValueTHDG->setVisible( true );
        ui->labelTHD2->setText("THD (Goerzel):");
    }
    else
    {
        ui->labelTHD1->setText("IMD3:");
        ui->labelValueTHDG->setVisible( false );
        ui->labelTHD2->setVisible( false );
        ui->labelSNR->setVisible( false );
        ui->labelValueSNR->setVisible( false );
    }
    if( m_sigType == SignalGenerator::eSNTest ||
        m_sigType == SignalGenerator::eEnd )
    {
        ui->labelSNR->setVisible( true );
        ui->labelValueSNR->setVisible( true );
    }
    else
    {
        ui->labelSNR->setVisible( false );
        ui->labelValueSNR->setVisible( false );
    }
}

void AnalyzerWorkerThread::run()
{
    while( m_run )
    {
        m_formAnalyzer->processAnalyzerData();
    }
}
