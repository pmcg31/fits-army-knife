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
      maxLabel("max: --")
{
    bottomLayout.addWidget(&minLabel);
    bottomLayout.addWidget(&maxLabel);

    layout.addWidget(&fitsWidget);
    layout.addLayout(&bottomLayout);

    setCentralWidget(&mainPane);

    QObject::connect(&fitsWidget, &FITSWidget::fileChanged,
                     this, &MainWindow::fitsFileChanged);
    QObject::connect(&fitsWidget, &FITSWidget::fileFailed,
                     this, &MainWindow::fitsFileFailed);

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
        sprintf(tmp, "min: %d", minPixelVal.i8);
        minLabel.setText(tmp);
        sprintf(tmp, "max: %d", maxPixelVal.i8);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_INT_16:
        sprintf(tmp, "min: %d", minPixelVal.i16);
        minLabel.setText(tmp);
        sprintf(tmp, "max: %d", maxPixelVal.i16);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_INT_32:
        sprintf(tmp, "min: %d", minPixelVal.i32);
        minLabel.setText(tmp);
        sprintf(tmp, "max: %d", maxPixelVal.i32);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_FLOAT:
        sprintf(tmp, "min: %0.4f", minPixelVal.f32);
        minLabel.setText(tmp);
        sprintf(tmp, "max: %0.4f", maxPixelVal.f32);
        maxLabel.setText(tmp);
        break;
    case ELS::FITS::BD_DOUBLE:
        sprintf(tmp, "min: %0.4lf", minPixelVal.f64);
        minLabel.setText(tmp);
        sprintf(tmp, "max: %0.4lf", maxPixelVal.f64);
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
