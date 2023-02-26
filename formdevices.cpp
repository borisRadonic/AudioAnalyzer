#include "formdevices.h"
#include "ui_formdevices.h"
#include <QSettings>
#include <QMessageBox>

#include "rtaudio\RtAudio.h"

#define MIN_SAMPLE_RATE 44100


#include <QDebug>

FormDevices::FormDevices( QSettings* settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormDevices),  
    m_inputDevice(1),
    m_outputDevice(1),
    m_sampleRate(48000),
    m_settings(settings)
{
    ui->setupUi(this);
    RtAudio::Api api;
    try
    {
        m_audio = new RtAudio( RtAudio::WINDOWS_WASAPI );
        api = m_audio->getCurrentApi();
    }
    catch (RtAudioError &error)
    {
        QMessageBox msg( QMessageBox::Critical, "Audio Interfaces Error", QString::fromStdString(error.getMessage()) );
        msg.exec();
        return;
    }

    // Determine the number of devices available
    int devices = m_audio->getDeviceCount();

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    int ii,io = 0;
    for (int jj=1; jj<=devices; jj++)
    {
        try
        {
            info = m_audio->getDeviceInfo(jj);
        }
        catch (RtAudioError &error)
        {
            qDebug() << QString::fromStdString(error.getMessage());
            break;
        }
        if( info.outputChannels > 0 )
        {
            ui->comboBoxDevOut->insertItem( io, QString::fromStdString( info.name ), jj );
            io++;
        }
        if( info.inputChannels > 0 )
        {
            ui->comboBoxDevIn->insertItem( ii, QString::fromStdString( info.name ), jj );
            ii++;
        }

    }

    connect( ui->buttonBox, SIGNAL(  accepted()),     SLOT(accepted()));
    connect( ui->buttonBox, SIGNAL(  rejected()),     SLOT(rejected()));

    connect( ui->comboBoxDevOut,  SIGNAL(  currentIndexChanged(int)),   SLOT(outputDeviceChanged()));
    connect( ui->comboBoxDevIn,   SIGNAL(  currentIndexChanged(int)),   SLOT(inputDeviceChanged()));

    readConfig();

}

FormDevices::~FormDevices()
{
    delete ui;
    if( m_audio )
    {
        delete m_audio;
    }
}

void FormDevices::readConfig()
{
    int data = m_settings->value( "DevicesInput", 1 ).toInt() ;
    int index = ui->comboBoxDevIn->findData(data);
    if( index > 0 )
    {
        ui->comboBoxDevIn->setCurrentIndex(index);
    }

    data = m_settings->value( "DevicesOutput", 1 ).toInt();
    index = ui->comboBoxDevOut->findData(data);
    if( index > 0 )
    {
        ui->comboBoxDevOut->setCurrentIndex(index);
    }

    int value = m_settings->value( "DevicesSampleRate", 48000 ).toInt() ;

    inputDeviceChanged();
    outputDeviceChanged();

    ui->comboBoxSampleRate->setCurrentIndex( ui->comboBoxSampleRate->findText(QString::number(value)));

}

void FormDevices::insertSampleRates( std::vector<unsigned int>& sampleRates )
{
    int k = 0;
    for( size_t i = 0; i < sampleRates.size(); i++ )
    {
        if( sampleRates[i] >= MIN_SAMPLE_RATE )
        {
            ui->comboBoxSampleRate->insertItem( k, QString::number(sampleRates[i]));
            k++;
        }
    }
}

void FormDevices::accepted()
{
    m_settings->setValue( "DevicesInput", ui->comboBoxDevIn->currentData().toInt() );
    m_settings->setValue( "DevicesOutput", ui->comboBoxDevOut->currentData().toInt() );
    m_settings->setValue( "DevicesSampleRate", ui->comboBoxSampleRate->currentText().toInt() );
    m_inputDevice = ui->comboBoxDevIn->currentData().toInt();
    m_outputDevice = ui->comboBoxDevOut->currentData().toInt();
    m_sampleRate = ui->comboBoxSampleRate->currentText().toInt();
    close();
}

void FormDevices::rejected()
{
    close();
}

void FormDevices::outputDeviceChanged()
{
    int dev = ui->comboBoxDevOut->currentData().toInt();
    RtAudio::DeviceInfo info;
    try
    {
        info = m_audio->getDeviceInfo(dev);
        insertSampleRates( info.sampleRates );

    }
    catch (RtAudioError &error)
    {
        qDebug() << QString::fromStdString(error.getMessage());
    }
}

void FormDevices::inputDeviceChanged()
{
    int dev = ui->comboBoxDevIn->currentData().toInt();
    RtAudio::DeviceInfo info;
    try
    {
        info = m_audio->getDeviceInfo(dev);
        insertSampleRates( info.sampleRates );

    }
    catch (RtAudioError &error)
    {
        qDebug() << QString::fromStdString(error.getMessage());
    }
}


