#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>

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
    MainWindow(QList<QFileInfo> fileList,
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

private:
    QList<QFileInfo> fileList;
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
    ELS::PixSTFParms stfParms;
};
#endif // MAINWINDOW_H
