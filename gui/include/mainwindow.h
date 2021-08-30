#pragma once

#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTcpServer>
#include <QVBoxLayout>

#include "imagefilelistitem.h"
#include "imagewidget.h"
#include "histogramwidget.h"
#include "pixstatistics.h"
#include "pixstfparms.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QTcpServer& server,
               QList<ImageFileListItem> fileList,
               QWidget* parent = nullptr);
    ~MainWindow();

private:
    // void fitsFileChanged(const char* filename);
    // void fitsFileFailed(const char* filename,
    //                     const char* errText);
    void imageZoomChanged(float zoom);

    void stretchToggled(bool isChecked);

    void zoomFitClicked(bool isChecked);
    void zoom100Clicked(bool isChecked);

    void prevClicked(bool isChecked);
    void nextClicked(bool isChecked);

    void newConnection();
    void acceptError(QAbstractSocket::SocketError error);
    void readyRead();
    void disconnected();

    void syncFileIdx();
    void syncFileCount();
    void syncStretch();

    // void addFilesToList(QList<QString> absoluteFilePaths);

private:
    QTcpServer& server;
    QList<QTcpSocket*> clients;
    QList<ImageFileListItem> fileList;
    QString filename;
    int currentFileIdx;
    bool showingStretched;
    QWidget mainPane;
    QVBoxLayout layout;
    ImageWidget imageWidget;
    HistogramWidget histWidget;
    QHBoxLayout bottomLayout;
    QHBoxLayout statsHistLayout;
    QVBoxLayout statsLayout;
    QLabel minLabel;
    QLabel meanLabel;
    QLabel medLabel;
    QLabel maxLabel;
    QLabel currentZoom;
    QIcon onIcon;
    QIcon offIcon;
    QPushButton stretchBtn;
    QPushButton zoomFitBtn;
    QPushButton zoom100Btn;
    QPushButton prevBtn;
    QPushButton nextBtn;
    QLabel fileListPosLabel;
    // ELS::PixSTFParms stfParms;
};
