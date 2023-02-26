#ifndef FORMDEVICES_H
#define FORMDEVICES_H

#include <QDialog>

#include <QAudioDeviceInfo>

namespace Ui {
class FormDevices;
}

class QSettings;
class RtAudio;

class FormDevices : public QDialog
{
    Q_OBJECT

public:
    explicit FormDevices( QSettings* settings, QWidget *parent = 0);

    ~FormDevices();

    inline int getInputDevice()
    {
        return m_inputDevice;
    }

    inline int getOutputDevice()
    {
        return m_outputDevice;
    }

    inline int getSampleRate()
    {
        return m_sampleRate;
    }

private slots:

    void accepted();
    void rejected();
    void outputDeviceChanged();
    void inputDeviceChanged();

private:

    void readConfig();
    void insertSampleRates( std::vector<unsigned int>& sampleRates );

private:
    Ui::FormDevices *ui;
    QSettings* m_settings;

    RtAudio *m_audio;
    int m_inputDevice;
    int m_outputDevice;
    int m_sampleRate;

};

#endif // FORMDEVICES_H
