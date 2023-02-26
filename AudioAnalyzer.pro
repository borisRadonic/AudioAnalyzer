#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T13:17:50
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioAnalyzer
TEMPLATE = app

INCLUDEPATH += C:\Qwt-6.1.2\src

SOURCES += main.cpp\
        mainwindow.cpp \
    formdevices.cpp \
    signalgenerator.cpp \
    formscope.cpp \
    scopeplotcurve.cpp \
    scopeplotbase.cpp \
    analogplot.cpp \
    formanalyzer.cpp \
    window.cpp \
    AnalyzerPlot.cpp \
    frequencyplot.cpp \
    formsiggen.cpp \
    audiofactory.cpp \
    audiointerface.cpp \
    RtAudio/RtAudio.cpp \
    RtAudio/include/asio.cpp \
    RtAudio/include/asiodrivers.cpp \
    RtAudio/include/asiolist.cpp \
    RtAudio/include/iasiothiscallresolver.cpp \
    four.cpp \
    weightingfilter.cpp \
    randomnumber.cpp

HEADERS  += mainwindow.h \
    formdevices.h \
    signalgenerator.h \
    formscope.h \
    signalbuffer.h \
    analogsignal.h \
    scopeplotcurve.h \
    scopeplot.h \
    scopeplotbase.h \
    analogplot.h \
    iscope.h \
    formanalyzer.h \
    window.h \
    IAnalyzer.h \
    AnalyzerPlot.h \
    frequencyplot.h \
    formsiggen.h \
    audiofactory.h \
    audiointerface.h \
    RtAudio/RtAudio.h \
    RtAudio/include/asio.h \
    RtAudio/include/asiodrivers.h \
    RtAudio/include/asiodrvr.h \
    RtAudio/include/asiolist.h \
    RtAudio/include/asiosys.h \
    RtAudio/include/dsound.h \
    RtAudio/include/FunctionDiscoveryKeys_devpkey.h \
    RtAudio/include/ginclude.h \
    RtAudio/include/iasiodrv.h \
    RtAudio/include/iasiothiscallresolver.h \
    RtAudio/include/soundcard.h \
    four.h \
    weightingfilter.h \
    randomnumber.h

FORMS    += mainwindow.ui \
    formdevices.ui \
    formscope.ui \
    formanalyzer.ui \
    formsiggen.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ -lqwtd -lole32 -ldsound -lwinmm
else:unix: LIBS += -L$$PWD/libs/ -lqwt

INCLUDEPATH += $$PWD/libs
DEPENDPATH += $$PWD/libs
