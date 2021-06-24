QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 debug

LIBS += -lcfitsio

INCLUDEPATH += \
    fits/include \
    gui/include \
    stf/include

SOURCES += \
    fits/src/fitsexception.cpp \
    fits/src/fitsimage.cpp \
    fits/src/fitstantrum.cpp \
    gui/src/main.cpp \
    gui/src/mainwindow.cpp \
    gui/src/fitswidget.cpp \
    gui/src/histogramwidget.cpp

HEADERS += \
    fits/include/fits.h \
    fits/include/fitsexception.h \
    fits/include/fitstantrum.h \
    fits/include/fitsimage.h \
    fits/include/fitspixelvisitor.h \
    fits/include/statisticsvisitor.h \
    stf/include/adaptivedisplayfuncvisitor.h \
    gui/include/mainwindow.h \
    gui/include/fitswidget.h \
    gui/include/histogramwidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target





