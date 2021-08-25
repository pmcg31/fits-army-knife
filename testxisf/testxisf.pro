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
    -lcrypto 

INCLUDEPATH += \
    $$PCL_INCLUDE_DIR 

SOURCES += \
    testxisf.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    $$PCL_INCLUDE_DIR/pcl/XISF.h 

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target





