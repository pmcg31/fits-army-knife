#include <QApplication>

#include "mainwindow.h"
#include "fits.h"
#include "fitsimage.h"
#include "fitsvariant.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mainPane(),
      layout(&mainPane),
      fitsWidget(),
      bottomLayout(),
      minLabel("min: --"),
      maxLabel("max: --"),
      currentZoom("--"),
      zoomFitBtn("fit"),
      zoom100Btn("1:1")
{
    const QSize btnSize(30, 30);
    const char *btnStyle = "QPushButton{border: none;border-radius: 7px;background-color: #444;}";
    const char *lblStyle = "QLabel{border: 1px solid #666;border-radius: 7px;color: #999;}";
    zoomFitBtn.setEnabled(true);
    zoomFitBtn.setStyleSheet(btnStyle);
    zoomFitBtn.setMinimumSize(btnSize);
    zoomFitBtn.setMaximumSize(btnSize);
    zoom100Btn.setEnabled(true);
    zoom100Btn.setStyleSheet(btnStyle);
    zoom100Btn.setMinimumSize(btnSize);
    zoom100Btn.setMaximumSize(btnSize);

    currentZoom.setStyleSheet(lblStyle);
    currentZoom.setAlignment(Qt::AlignCenter);
    currentZoom.setMinimumWidth(65);

    minLabel.setStyleSheet(lblStyle);
    minLabel.setAlignment(Qt::AlignCenter);
    maxLabel.setStyleSheet(lblStyle);
    maxLabel.setAlignment(Qt::AlignCenter);

    bottomLayout.addWidget(&minLabel);
    bottomLayout.addWidget(&maxLabel);
    bottomLayout.addStretch(1);
    bottomLayout.addWidget(&zoomFitBtn);
    bottomLayout.addWidget(&zoom100Btn);
    bottomLayout.addWidget(&currentZoom);

    layout.addWidget(&fitsWidget);
    layout.addLayout(&bottomLayout);

    setCentralWidget(&mainPane);

    QObject::connect(&fitsWidget, &FITSWidget::fileChanged,
                     this, &MainWindow::fitsFileChanged);
    QObject::connect(&fitsWidget, &FITSWidget::fileFailed,
                     this, &MainWindow::fitsFileFailed);
    QObject::connect(&fitsWidget, &FITSWidget::actualZoomChanged,
                     this, &MainWindow::fitsZoomChanged);
    QObject::connect(&zoomFitBtn, &QPushButton::clicked,
                     this, &MainWindow::zoomFitClicked);
    QObject::connect(&zoom100Btn, &QPushButton::clicked,
                     this, &MainWindow::zoom100Clicked);

    QStringList args = QApplication::arguments();
    if (args.length() < 2)
    {
        printf("No file specified\n");
        fflush(stdout);
    }
    else
    {
        if (args.length() == 2)
        {
            const char *filename = args.at(1).toLocal8Bit().constData();
            printf("Setting file %s\n", filename);
            fflush(stdout);
            fitsWidget.setFile(filename);
        }
        else
        {
            fprintf(stderr, "Your plethora of arguments has confused me\n");
            fflush(stderr);
        }
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::fitsFileChanged(const char *filename)
{
    printf("File loaded: %s\n", filename);

    const ELS::FITSImage *image = fitsWidget.getImage();
    ELS::FITSVariant minPixelVal = image->getMinPixelVal();
    ELS::FITSVariant maxPixelVal = image->getMaxPixelVal();
    char tmp[20];
    switch (image->getBitDepth())
    {
    case ELS::FITS::BD_INT_8:
        sprintf(tmp, " min: %d ", minPixelVal.i8);
        minLabel.setText(tmp);
        sprintf(tmp, " max: %d ", maxPixelVal.i8);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_INT_16:
        sprintf(tmp, " min: %d ", minPixelVal.i16);
        minLabel.setText(tmp);
        sprintf(tmp, " max: %d ", maxPixelVal.i16);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_INT_32:
        sprintf(tmp, " min: %d ", minPixelVal.i32);
        minLabel.setText(tmp);
        sprintf(tmp, " max: %d ", maxPixelVal.i32);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_FLOAT:
        sprintf(tmp, " min: %0.4f ", minPixelVal.f32);
        minLabel.setText(tmp);
        sprintf(tmp, " max: %0.4f ", maxPixelVal.f32);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_DOUBLE:
        sprintf(tmp, " min: %0.4lf ", minPixelVal.f64);
        minLabel.setText(tmp);
        sprintf(tmp, " max: %0.4lf ", maxPixelVal.f64);
        maxLabel.setText(tmp);
        break;
    }

    printf("%s\n", image->getImageType());
    printf("%s\n", image->getSizeAndColor());
    fflush(stdout);
}

void MainWindow::fitsFileFailed(const char *filename,
                                const char *errText)
{
    printf("File failed [%s]: %s\n", errText, filename);
}

void MainWindow::fitsZoomChanged(float zoom)
{
    char tmp[10];

    sprintf(tmp, "%.1f%%", zoom * 100);
    currentZoom.setText(tmp);
}

void MainWindow::zoomFitClicked(bool /* isChecked */)
{
    fitsWidget.setZoom(-1.0);
}

void MainWindow::zoom100Clicked(bool /* isChecked */)
{
    fitsWidget.setZoom(1.0);
}
