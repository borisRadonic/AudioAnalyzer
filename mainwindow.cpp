#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formdevices.h"
#include "formsiggen.h"
#include "formscope.h"
#include "formanalyzer.h"
#include "audiofactory.h"
#include "audiointerface.h"
#include <QMessageBox.h>
#include <QtCore/QSettings.h>
#include <QtCore/QProcess>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTimer>
#include <QSignalMapper>
#include <QMessageBox>
#include <QCloseEvent>
#include "math.h"
#include "rtaudio\RtAudio.h"

namespace
{
    const QString INI_PREFIX_SIG_GEN= "FormSignalGenerator";

    const QString INI_PREFIX_SCOPE  = "FormScope" ;
    const QPoint INI_DEFAULT_SCOPE_POS(0,0);
    const QSize INI_DEFAULT_SCOPE_SIZE(564,367);

    const QString INI_PREFIX_SIGGEN  = "FormSigGen" ;
    const QPoint INI_DEFAULT_SIGGEN_POS(0,0);
    const QSize INI_DEFAULT_SIGGEN_SIZE(400,245);

    const QString INI_PREFIX_ANALYZER= "FormAnalyzer";
    const QPoint INI_DEFAULT_ANALYZER_POS(565,0);
    const QSize INI_DEFAULT_ANALYZER_SIZE(663,437);

    const QPoint INI_DEFAULT_MAINWINDOW_POS(32,49);
    const QSize INI_DEFAULT_MAINWINDOW_SIZE(1232,889);
}

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audio( NULL ),
    m_audioInterface( NULL ),
    m_formScope(NULL),
    m_formSigGen(NULL),
    m_formAnalyzer(NULL),
    m_connected( false ),
    m_currentSampleRate(96000),
    m_currentOutputDevice(1),
    m_currentInputDevice(1)
{
    ui->setupUi(this);

    m_config = new QSettings( "AudioAnalyzer.ini", QSettings::IniFormat );

    m_pMdiArea = new QMdiArea;
    m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_pMdiArea);

    m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    QObject::connect(ui->actionOscilloscope,     SIGNAL(triggered(bool)), this, SLOT(showFormScope(bool)));
    QObject::connect(ui->actionSignalGenerator,  SIGNAL(triggered(bool)), this, SLOT(showFormSigGen(bool)));
    QObject::connect(ui->actionSpectrumAnalyzer, SIGNAL(triggered(bool)), this, SLOT(showFormAnalyzer(bool)));
    QObject::connect(ui->actionDevices,          SIGNAL(triggered()), this, SLOT(showAudioDevices()));
    QObject::connect(ui->actionConnect,          SIGNAL(triggered()), this, SLOT(onConnect()));
    QObject::connect(ui->actionDisconnect,       SIGNAL(triggered()), this, SLOT(onDisconnect()));

    createScopeForm();    
    createAnalyzerForm();

    readSettings();

    //this is test
    m_testTimer = new QTimer();

    m_testTimer->start( 10 );
    connect(m_testTimer, SIGNAL(timeout()), SLOT(onTestTimer()));
    time = 0;

}

MainWindow::~MainWindow()
{
    delete m_config;
    delete m_testTimer;
    delete ui;    
}

void MainWindow::setFormScopeClosed()
{
    writeWidgetSettings( INI_PREFIX_SCOPE, m_formScope, true );
    m_pMdiArea->removeSubWindow( m_formScope );
    m_audioInterface->disconnectScope();
    m_formScope = NULL;
}

void MainWindow::setFormSigGenClosed()
{
    m_formSigGen->disconnectromSignalGenerator();
    writeWidgetSettings( INI_PREFIX_SIGGEN, m_formSigGen, true );
    m_pMdiArea->removeSubWindow( m_formSigGen );
    m_formSigGen = NULL;
    if( m_formAnalyzer )
    {
        m_formAnalyzer->resetSigGenParameters();
    }
}

void MainWindow::setFormAnalyzerClosed()
{
    writeWidgetSettings( INI_PREFIX_ANALYZER, m_formAnalyzer, true );
    m_pMdiArea->removeSubWindow( m_formAnalyzer );
    m_audioInterface->disconnectAnalyzer();
    m_formAnalyzer = NULL;
}

void MainWindow::formSigGenFreqChanged( SignalGenerator::SignalType sigType, unsigned int freq1, unsigned int freq2 )
{
    if( m_formAnalyzer )
    {
         m_formAnalyzer->setSigGenParameters( sigType, freq1, freq2 );
    }
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
    {
        return;
    }
    m_pMdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::closeEvent(QCloseEvent *event)
{   
    writeSettings();
    m_pMdiArea->closeAllSubWindows();
    event->accept();
}

void MainWindow::onTestTimer()
{
}

void MainWindow::readWidgetSettings( const QString prefix, QWidget* widget, const QPoint defaultPos, const QSize defaultSize )
{    
    QPoint pos = m_config->value( prefix + "Pos", defaultPos ).toPoint();
    widget->parentWidget()->move(pos);
    QSize size = m_config->value( prefix + "Size", defaultSize ).toSize();
    widget->parentWidget()->resize(size);
}

void MainWindow::writeWidgetSettings( const QString prefix, const QWidget* widget, bool visible )
{
    if( visible )
    {
        m_config->setValue( prefix + "Visible", true );
        m_config->setValue( prefix + "Pos", widget->parentWidget()->pos() );
        m_config->setValue( prefix + "Size", widget->parentWidget()->size() );

    }
    else
    {
        m_config->setValue( prefix + "Visible", false );
    }
}

void MainWindow::readSettings()
{
    QPoint pos = m_config->value( "MWP", INI_DEFAULT_MAINWINDOW_POS ).toPoint();
    QSize size = m_config->value( "MWS", INI_DEFAULT_MAINWINDOW_SIZE ).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    m_config->setValue( "MWP", pos());
    m_config->setValue( "MWS", size());
    if( NULL != m_formScope )
    {
        writeWidgetSettings( INI_PREFIX_SCOPE, m_formScope, true );
    }
    else
    {
        writeWidgetSettings( INI_PREFIX_SCOPE, NULL, false );
    }
    if( NULL != m_formAnalyzer )
    {
        writeWidgetSettings( INI_PREFIX_ANALYZER, m_formAnalyzer, true );
    }
    else
    {
        writeWidgetSettings( INI_PREFIX_ANALYZER, NULL, false );
    }
    if( NULL != m_formSigGen )
    {
        writeWidgetSettings( INI_PREFIX_SIGGEN, m_formSigGen, true );
    }
    else
    {
        writeWidgetSettings( INI_PREFIX_SIGGEN, NULL, false );
    }
}

void MainWindow::showFormScope(bool /*checked*/)
{
    if( NULL == m_formScope )
    {
        createScopeForm();
        if( m_connected && m_audioInterface && m_formScope )
        {
            m_audioInterface->connectScope( (IScope*) m_formScope );
            m_formScope->setSampleTime( 1.0/m_currentSampleRate );
        }
    }
}


void MainWindow::showFormSigGen(bool /*checked*/)
{
    if(  !m_connected )
    {
        QMessageBox msg( QMessageBox::Warning, "Connection Problem", "You are not connected to audio interface." );
        msg.exec();
        return;
    }
    if( NULL == m_formSigGen )
    {
        createSigGenForm();
    }

    if( m_formAnalyzer && m_formSigGen )
    {
        SignalGenerator::SignalType sigType;
        unsigned int freq1;
        unsigned int freq2;
        m_formSigGen->getSigGenParams(sigType, freq1, freq2 );
        m_formAnalyzer->setSigGenParameters( sigType, freq1, freq2 );
    }
}

void MainWindow::showFormAnalyzer(bool/*checked*/)
{
    if( NULL == m_formAnalyzer )
    {
        createAnalyzerForm();
        if( m_connected && m_audioInterface && m_formAnalyzer )
        {
            m_audioInterface->connectAnalyzer( (IAnalyzer*) m_formAnalyzer );
        }
    }
}

void MainWindow::showAudioDevices()
{
    if( !m_connected )
    {
        FormDevices devices(m_config,this);
        devices.exec();
        readAudioParameters();
        if( m_formScope )
        {
            m_formScope->setSampleTime( 1.0/m_currentSampleRate );
        }
        if( m_formAnalyzer )
        {
            m_formAnalyzer->setSamplingRate( m_currentSampleRate );
        }
    }
}

void MainWindow::onConnect()
{
    if( !m_connected )
    {
        readAudioParameters();

        try
        {
            m_audio = AudioFactory::CreateAudio();
        }
        catch (RtAudioError &error)
        {
            QMessageBox msg( QMessageBox::Warning, "Connection Problem", QString::fromStdString( error.getMessage() ) );
            msg.exec();
            return;
        }
        m_audioInterface = new AudioInterface(  m_audio, m_currentSampleRate, m_currentInputDevice, m_currentOutputDevice, this );
        m_connected = true;
        if( !m_formScope )
        {
            createScopeForm();
        }
        if( m_formScope )
        {
            m_audioInterface->connectScope( (IScope*) m_formScope );
            m_formScope->setSampleTime( 1.0/m_currentSampleRate );
        }
        if( !m_formAnalyzer )
        {
            createAnalyzerForm();
        }
        if( m_formAnalyzer )
        {
            m_formAnalyzer->setSamplingRate( m_currentSampleRate );
            m_audioInterface->connectAnalyzer( (IAnalyzer*) m_formAnalyzer );
        }

        createSigGenForm();
        //
    }
}

void MainWindow::onDisconnect()
{
    if( m_connected )
    {
        if( m_audioInterface )
        {
            m_audioInterface->disconnectAnalyzer();
            m_audioInterface->disconnectScope();
            m_audioInterface->stopAudio();
        }
        m_connected = false;
    }
}

void MainWindow::createScopeForm()
{
    m_formScope = new FormScope( m_config, this );
    m_pMdiArea->addSubWindow(m_formScope);
    m_formScope->setSampleTime( 1.0/m_currentSampleRate );
    m_formScope->connectToScope();
    m_formScope->show();
    readWidgetSettings( INI_PREFIX_SCOPE,	m_formScope, INI_DEFAULT_SCOPE_POS,	INI_DEFAULT_SCOPE_SIZE );
}

void MainWindow::createSigGenForm()
{
    m_formSigGen = new FormSigGen( m_config, m_audioInterface, this );
    m_pMdiArea->addSubWindow(m_formSigGen);
    m_formSigGen->show();
    readWidgetSettings( INI_PREFIX_SIGGEN,	m_formSigGen, INI_DEFAULT_SIGGEN_POS,	INI_DEFAULT_SIGGEN_SIZE );
    m_formSigGen->connectToSignalGenerator( m_audio, m_audioInterface, m_currentSampleRate);
}


void MainWindow::createAnalyzerForm()
{
    m_formAnalyzer = new FormAnalyzer( m_config, this );
    m_pMdiArea->addSubWindow(m_formAnalyzer);
    m_formAnalyzer->setSamplingRate( m_currentSampleRate );
    m_formAnalyzer->show();
    readWidgetSettings( INI_PREFIX_ANALYZER,	m_formAnalyzer, INI_DEFAULT_ANALYZER_POS,	INI_DEFAULT_ANALYZER_SIZE );
}

void MainWindow::readAudioParameters()
{
    m_currentInputDevice = m_config->value( "DevicesInput", 1 ).toInt();
    m_currentOutputDevice = m_config->value( "DevicesOutput", 1 ).toInt();
    m_currentSampleRate = m_config->value( "DevicesSampleRate", 48000 ).toInt();
}


