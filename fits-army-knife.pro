QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 

LIBS += -lcfitsio

INCLUDEPATH += \
    fits/include \
    gui/include \
    raster/include

SOURCES += \
    fits/src/fitsexception.cpp \
    fits/src/fitsimage.cpp \
    fits/src/fitstantrum.cpp \
    raster/src/pixutils.cpp \
    raster/src/pixstfparms.cpp \
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
    raster/include/pixutils.h \
    raster/include/pixstatistics.h \
    raster/include/pixstfparms.h \
    raster/include/adaptivedisplayfuncvisitor.h \
    gui/include/mainwindow.h \
    gui/include/fitswidget.h \
    gui/include/histogramwidget.h

RESOURCES += \
    icon/icon.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target





