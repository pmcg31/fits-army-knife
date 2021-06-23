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

private:
    void fitsFileChanged(const char *filename);
    void fitsFileFailed(const char *filename,
                        const char *errText);
    void fitsZoomChanged(float zoom);

    void zoomFitClicked(bool isChecked);
    void zoom100Clicked(bool isChecked);

private:
    QWidget mainPane;
    QVBoxLayout layout;
    FITSWidget fitsWidget;
    HistogramWidget histWidget;
    QHBoxLayout bottomLayout;
    QVBoxLayout statsLayout;
    QHBoxLayout statsHistLayout;
    QLabel minLabel;
    QLabel meanLabel;
    QLabel medLabel;
    QLabel maxLabel;
    QLabel currentZoom;
    QPushButton zoomFitBtn;
    QPushButton zoom100Btn;
};
#endif // MAINWINDOW_H
