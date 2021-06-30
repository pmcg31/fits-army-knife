#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QTcpServer>

#include "pixstatistics.h"
#include "pixstfparms.h"
#include "fitswidget.h"
#include "histogramwidget.h"

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
    MainWindow(QTcpServer &server,
               QList<QFileInfo> fileList,
               QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void showStretched(ELS::PixSTFParms stfParms);
    void clearStretched();

private:
    void fitsFileChanged(const char *filename);
    void fitsFileFailed(const char *filename,
                        const char *errText);
    void fitsZoomChanged(float zoom);

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

    void addFilesToList(QList<QString> absoluteFilePaths);

private:
    QTcpServer &server;
    QList<QTcpSocket *> clients;
    QList<QFileInfo> fileList;
    char filename[1000];
    int currentFileIdx;
    bool showingStretched;
    QWidget mainPane;
    QVBoxLayout layout;
    FITSWidget fitsWidget;
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
    ELS::PixSTFParms stfParms;
};
#endif // MAINWINDOW_H
