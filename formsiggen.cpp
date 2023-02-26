#include "formsiggen.h"
#include "ui_formsiggen.h"
#include "audiointerface.h"
#include "mainwindow.h"
#include "randomnumber.h"
#include <QDebug>
#include <QCloseEvent>
#include <QSettings>
#include <qwt_plot.h>
#include <qwt_scale_map.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_math.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qframe.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_canvas.h>
#include <QColorDialog>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_text.h>
#include <qmath.h>

FormSigGen::FormSigGen( QSettings* settings, AudioInterface* audioInterace, QWidget *parent ) :
QWidget(parent),
ui(new Ui::FormSigGen),
m_audio( NULL ),
m_audioInterace( audioInterace ),
m_mainWindow( (MainWindow*) parent),
m_settings( settings ),
m_currentSampleRate(0),
m_out1(true),
m_out2(false),
m_sigType(SignalGenerator::eEnd),
m_freq1(0),
m_freq2(0),
m_phi1(0.0),
m_phi2(0.0),
m_amp1(0.0),
m_amp2(0.0)
{
    ui->setupUi(this);

    m_plot = new QwtPlot( ui->widgetGraph );
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 3 );

    m_randomGen = new RandomNumber();

    m_plot->setCanvas( canvas );
    m_plot->setCanvasBackground( QColor( "Black" ) );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setMajorPen( QColor::fromRgb( 255,255,255), 0.3, Qt::SolidLine);
    grid->setMinorPen( QColor::fromRgb( 255,255,255), 0.3, Qt::DotLine);
    grid->attach( (QwtPlot*) m_plot);
    m_plot->setAxisScale( QwtPlot::yLeft, -1.0, 1.0, 0.5 );
    m_plot->enableAxis(  QwtPlot::xBottom, false );
    m_plot->enableAxis(  QwtPlot::yLeft, false );

    m_pPlotCurve = new QwtPlotCurve( "" );
    m_pPlotCurve->attach( m_plot );
    m_pPlotCurve->setPen(Qt::yellow);
    m_pPlotCurve->setStyle(QwtPlotCurve::Lines);

    ui->comboBoxFunc->insertItem( (int) SignalGenerator::eSinus,        "sin" );
    ui->comboBoxFunc->insertItem( (int) SignalGenerator::eSinPlusSin,   "TestIMD" );
    ui->comboBoxFunc->insertItem( (int) SignalGenerator::eSNTest,       "Test SNR" );
    ui->comboBoxFunc->insertItem( (int) SignalGenerator::eWhiteNoise,   "White N" );
    //ui->comboBoxFunc->insertItem( (int) SignalGenerator::ePinkNoise,    "Pink N" );
    //ui->comboBoxFunc->insertItem( (int) SignalGenerator::eSweep,        "Sweep" );

    connect( ui->comboBoxFunc,           SIGNAL(  currentIndexChanged(int)),     SLOT(functionChanged(int)));
    connect( ui->spinBoxFreq1,           SIGNAL(  valueChanged(int)),            SLOT(freq1Changed(int)));
    connect( ui->spinBoxFreq2,           SIGNAL(  valueChanged(int)),            SLOT(freq2Changed(int)));
    connect( ui->spinBoxPhi1,            SIGNAL(  valueChanged(int)),            SLOT(phi1Changed(int)));
    connect( ui->spinBoxPhi2,            SIGNAL(  valueChanged(int)),            SLOT(phi2Changed(int)));
    connect( ui->doubleSpinBoxAmp1,      SIGNAL(  valueChanged(double)),         SLOT(doubleSpinBoxAmp1Changed(double)));
    connect( ui->doubleSpinBoxAmp2,      SIGNAL(  valueChanged(double)),         SLOT(doubleSpinBoxAmp2Changed(double)));
    connect( ui->pushButtonStartStop,    SIGNAL(  toggled(bool) ),               SLOT(startPressed(bool)));
    connect( ui->checkBoxCh1,            SIGNAL(  stateChanged(int) ),           SLOT(outChannel1Changed(int)));
    connect( ui->checkBoxCh2,            SIGNAL(  stateChanged(int) ),           SLOT(outChannel2Changed(int)));

    setDefaults();
    m_mainWindow->formSigGenFreqChanged( m_sigType, m_freq1, m_freq2 );

}

FormSigGen::~FormSigGen()
{
    delete m_pPlotCurve;
    delete m_plot;
    delete m_randomGen;
    delete ui;

}

void FormSigGen::setDefaults()
{
    int index = m_settings->value( "SigGenFunc", 0 ).toInt();
    ui->comboBoxFunc->setCurrentIndex( index );
    functionChanged(index);

    double dValue = m_settings->value( "SigGenAmp1", -5 ).toDouble();
    ui->doubleSpinBoxAmp1->setValue(dValue);
    doubleSpinBoxAmp1Changed(dValue);

    dValue  = m_settings->value( "SigGenAmp2", -17 ).toDouble();
    ui->doubleSpinBoxAmp2->setValue(dValue);
    doubleSpinBoxAmp2Changed(dValue);

    int iValue =  m_settings->value( "SigGenFreq1", 60 ).toInt();
    ui->spinBoxFreq1->setValue(iValue);
    freq1Changed(iValue);

    iValue =  m_settings->value( "SigGenFreq2", 60 ).toInt();
    ui->spinBoxFreq2->setValue(iValue);
    freq2Changed(iValue);

    iValue =  m_settings->value( "SigGenPhi1", 0 ).toInt();
    ui->spinBoxPhi1->setValue(iValue);
    phi1Changed(iValue);

    iValue =  m_settings->value( "SigGenPhi2", 0 ).toInt();
    ui->spinBoxPhi2->setValue(iValue);
    phi2Changed(iValue);

}

void FormSigGen::closeEvent(QCloseEvent*)
{
    if( m_audio && m_audioInterace )
    {
        m_audioInterace->disconnectAnalyzer();
        m_audioInterace->stopAudio();
    }
    m_settings->setValue( "SigGenFunc",     ui->comboBoxFunc->currentIndex() );
    m_settings->setValue( "SigGenAmp1",     ui->doubleSpinBoxAmp1->value() );
    m_settings->setValue( "SigGenAmp2",     ui->doubleSpinBoxAmp2->value() );
    m_settings->setValue( "SigGenFreq1",    ui->spinBoxFreq1->value() );
    m_settings->setValue( "SigGenFreq2",    ui->spinBoxFreq2->value() );
    m_settings->setValue( "SigGenPhi1",     ui->spinBoxPhi1->value() );
    m_settings->setValue( "SigGenPhi2",     ui->spinBoxPhi2->value() );
    m_mainWindow->setFormSigGenClosed();
}

void FormSigGen::resizeEvent(QResizeEvent*)
{
}

void FormSigGen::startPressed( bool start )
{
    if( start && m_audio && m_audioInterace)
    {
         m_audioInterace->startAudio( m_audio, m_currentSampleRate );
         ui->comboBoxFunc->setEnabled(false);
         ui->pushButtonStartStop->setText("Stop");
         m_mainWindow->formSigGenFreqChanged( m_sigType, m_freq1, m_freq2 );
    }
    else
    {
        if( m_audio && m_audioInterace )
        {
            m_audioInterace->stopAudio();
            ui->comboBoxFunc->setEnabled(true);
            ui->pushButtonStartStop->setText("Start");
        }
    }
    qWarning() << "FormSigGen::startPressed start= " << start;
}

void FormSigGen::enableAll( bool enabled )
{
    ui->spinBoxFreq1->setEnabled(enabled);
    ui->spinBoxFreq2->setEnabled(enabled);
    ui->doubleSpinBoxAmp1->setEnabled(enabled);
    ui->doubleSpinBoxAmp2->setEnabled(enabled);
    ui->spinBoxPhi1->setEnabled(enabled);
    ui->spinBoxPhi2->setEnabled(enabled);
    ui->labelFreq1->setText("Frequency [Hz]:");
    ui->labelPhi1->setText("Phi [deg]:");
}

void FormSigGen::showPlot()
{
    if( xData.size() > 0 )
    {
        xData.clear();
    }
    if( yData.size() > 0 )
    {
        yData.clear();
    }
    double t(0.0);
    double deltaT(0.0);
    double v(0.0);
    double nSteps = 200.00;
    if( m_sigType == SignalGenerator::eSinus && m_freq1 > 0)
    {
        deltaT = 1.0/ ((double) m_freq1 * nSteps);
        m_plot->setAxisScale(QwtPlot::xBottom, 0, deltaT * nSteps );
        for( unsigned int i = 0; i < ( unsigned int ) nSteps; i++ )
        {
            v = m_amp1 * sin( 2.0 * M_PI * m_freq1 * t + m_phi1 );
            t += deltaT;
            xData.push_back(t);
            yData.push_back(v);
        }
    }
    else if( m_sigType ==  SignalGenerator::eSinPlusSin )
    {
        double freq =  qMin( m_freq1, m_freq2 );;
        deltaT = 1.0/ ((double) freq * nSteps);
        m_plot->setAxisScale(QwtPlot::xBottom, 0, deltaT * nSteps );
        for( unsigned int i = 0; i < ( unsigned int ) nSteps; i++ )
        {
            v = m_amp1 * sin( 2.0 * M_PI * m_freq1 * t + m_phi1 ) + m_amp2 * sin( 2.0 * M_PI * m_freq2 * t + m_phi2 );
            t += deltaT;
            xData.push_back(t);
            yData.push_back(v);
        }
    }
    else if( m_sigType ==  SignalGenerator::eSNTest )
    {
        for( unsigned int i = 0; i < ( unsigned int ) nSteps; i++ )
        {
            xData.push_back(i);
            yData.push_back(0.0);
        }
    }
    else if( m_sigType ==  SignalGenerator::eWhiteNoise )
    {
        deltaT = 1.0/ ((double) nSteps);
        for( unsigned int i = 0; i < ( unsigned int ) nSteps; i++ )
        {
            t += deltaT;
            xData.push_back(t);
            yData.push_back( m_amp1 * 2 * m_randomGen->white(0.5) );
        }
    }


    m_pPlotCurve->setSamples( xData, yData );
    m_plot->replot();
}

void FormSigGen::functionChanged(int index)
{
    m_sigType = (SignalGenerator::SignalType) index;
    if( m_audioInterace )
    {
        m_audioInterace->setSignalType( m_sigType );
    }
    enableAll(true);
    switch(m_sigType)
    {
        case SignalGenerator::eSinus:
        {
            ui->doubleSpinBoxAmp2->setEnabled(false);
            ui->spinBoxFreq2->setEnabled(false);
            ui->spinBoxPhi2->setEnabled(false);
            break;
        }
        case SignalGenerator::eSinPlusSin:
        {
            ui->labelFreq1->setText("Frequency1 [Hz]:");
            ui->labelPhi1->setText("Phi1 [deg]:");
            break;
        }
        case SignalGenerator::eWhiteNoise:
        case SignalGenerator::ePinkNoise:
        {
            ui->doubleSpinBoxAmp2->setEnabled(false);
            ui->spinBoxFreq2->setEnabled(false);
            ui->spinBoxPhi2->setEnabled(false);
            ui->spinBoxFreq1->setEnabled(false);
            ui->spinBoxPhi1->setEnabled(false);
            break;
        }
        case SignalGenerator::eSweep:
        {
            enableAll(false);
            break;
        }
        default:
            break;
    }
    m_mainWindow->formSigGenFreqChanged( m_sigType, m_freq1, m_freq2 );
    showPlot();
    qWarning() << "FormSigGen::functionChanged";
}

void FormSigGen::freq1Changed(int value)
{
    if( m_audioInterace )
    {
        m_audioInterace->setFrequency1( value );
    }
    m_freq1 = (unsigned int ) value;
    m_mainWindow->formSigGenFreqChanged( m_sigType, m_freq1, m_freq2 );
    showPlot();
}

void FormSigGen::freq2Changed(int value)
{
    if( m_audioInterace )
    {
        m_audioInterace->setFrequency2( value );
    }
    m_freq2 = (unsigned int ) value;
    m_mainWindow->formSigGenFreqChanged( m_sigType, m_freq1, m_freq2 );
    showPlot();
}

void FormSigGen::phi1Changed(int value)
{
    m_phi1 = ((double) value * 2.0 * M_PI) /360.0;
    if( m_audioInterace )
    {
        m_audioInterace->setPhi1( m_phi1 );
    }
    showPlot();
}

void FormSigGen::phi2Changed(int value)
{
    m_phi2 = ((double) value * 2.0 * M_PI) /360.0;
    if( m_audioInterace )
    {
        m_audioInterace->setPhi2( m_phi2 );
    }
    showPlot();
}

void FormSigGen::doubleSpinBoxAmp1Changed(double value)
{
    m_amp1 = pow( 10, value/20 );
    if( m_audioInterace )
    {
        m_audioInterace->setAmp1(m_amp1);
    }
    showPlot();
}

void FormSigGen::doubleSpinBoxAmp2Changed(double value)
{
    m_amp2 = pow( 10, value/20 );
    {
        m_audioInterace->setAmp2(m_amp2);
    }
    showPlot();
}

void FormSigGen::outChannel1Changed( int state )
{
    m_out1 = ( state != 0 );
    m_audioInterace->setOut1( m_out1 );
}

void FormSigGen::outChannel2Changed( int state )
{
    m_out2 = ( state != 0 );
    m_audioInterace->setOut2( m_out2 );
}

