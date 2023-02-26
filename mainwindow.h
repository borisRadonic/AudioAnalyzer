#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "signalgenerator.h"

namespace Ui
{
class MainWindow;
}

class QMdiArea;
class FormScope;
class FormSigGen;
class FormAnalyzer;
class QTimer;
class QSignalMapper;
class RtAudio;
class AudioInterface;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow( QWidget *parent = 0 );

    ~MainWindow();

    void setFormScopeClosed();

    void setFormSigGenClosed();

    void setFormAnalyzerClosed();

    void formSigGenFreqChanged( SignalGenerator::SignalType sigType, unsigned int freq1, unsigned int freq2 );

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:

    void showFormScope(bool checked);
    void showFormSigGen(bool checked);
    void showFormAnalyzer(bool checked);
    void showAudioDevices();
    void onConnect();
    void onDisconnect();

    void onTestTimer();

    void setActiveSubWindow(QWidget *window);


private:


    void readSettings();

    void writeSettings();

    void readWidgetSettings( const QString prefix, QWidget* widget, const QPoint defaultPos, const QSize defaultSize );

    void writeWidgetSettings( const QString prefix, const QWidget* widget, bool visible );

    void createScopeForm();

    void createSigGenForm();

    void createAnalyzerForm();

    void readAudioParameters();

private:

    Ui::MainWindow*     ui;

    RtAudio*            m_audio;
    AudioInterface*     m_audioInterface;

    FormScope*          m_formScope;
    FormSigGen*         m_formSigGen;
    FormAnalyzer*       m_formAnalyzer;

    QSettings*          m_config;
    QMdiArea*           m_pMdiArea;
    QSignalMapper*      m_windowMapper;


    QTimer*             m_testTimer;
    double              time;

    bool                m_connected;
    unsigned int        m_currentSampleRate;
    unsigned int        m_currentOutputDevice;
    unsigned int        m_currentInputDevice;

};

#endif // MAINWINDOW_H
