#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void showStretched(const double *mBal,
                       const double *sClip,
                       const double *hClip,
                       const double *sExp,
                       const double *hExp);
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
    char filename[1000];
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
    double madn[3];
    double sClip[3];
    double hClip[3];
    double mBal[3];
    double sExp[3];
    double hExp[3];
};
#endif // MAINWINDOW_H
