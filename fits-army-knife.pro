QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 

# PixInsight Class Library
PCL_RELEASE_DIR = $$(PCLDIR)/src/pcl/linux/g++/x64/Release
PCL_RFC6234_RELEASE_DIR = $$(PCLDIR)/src/3rdparty/RFC6234/linux/g++/x64/Release
PCL_INCLUDE_DIR = $$(PCLDIR)/include

LIBS += \
    $$PCL_RELEASE_DIR/libPCL-pxi.a \
    $$PCL_RFC6234_RELEASE_DIR/libRFC6234-pxi.a \
    -lz \
    -llz4 \
    -llcms2 \
    -lcrypto \
    -lcfitsio

INCLUDEPATH += \
    $$PCL_INCLUDE_DIR \
    image/raster/include \
    image/fits/include \
    image/xisf/include \ 
    gui/include

SOURCES += \
    image/fits/src/fitsexception.cpp \
    image/fits/src/fitsimage.cpp \
    image/fits/src/fitstantrum.cpp \
    image/raster/src/image.cpp \
    image/raster/src/pixelvisitortypemismatch.cpp \
    image/raster/src/pixelvisitor.cpp \
    image/raster/src/pixutils.cpp \
    image/raster/src/pixstfparms.cpp \
    gui/src/main.cpp \
    gui/src/mainwindow.cpp \
    gui/src/imagewidget.cpp \
    gui/src/histogramwidget.cpp

HEADERS += \
    image/fits/include/fitsexception.h \
    image/fits/include/fitstantrum.h \
    image/fits/include/fitsimage.h \
    $$PCL_INCLUDE_DIR/pcl/XISF.h \
    image/raster/include/image.h \
    image/raster/include/rastertypes.h \
    image/raster/include/pixelvisitortypemismatch.h \
    image/raster/include/pixelvisitor.h \
    image/raster/include/pixutils.h \
    image/raster/include/pixstatistics.h \
    image/raster/include/pixstfparms.h \
    image/raster/include/statisticsvisitor.h \
    gui/include/mainwindow.h \
    gui/include/imagewidget.h \
    gui/include/histogramwidget.h

RESOURCES += \
    icon/icon.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target





