#include <QApplication>

#include "mainwindow.h"
#include "fits.h"
#include "fitsimage.h"
#include "statisticsvisitor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mainPane(),
      layout(&mainPane),
      fitsWidget(),
      histWidget(),
      bottomLayout(),
      minLabel(" min: -- "),
      meanLabel(" mean: -- "),
      maxLabel(" max: -- "),
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
    meanLabel.setStyleSheet(lblStyle);
    meanLabel.setAlignment(Qt::AlignCenter);
    maxLabel.setStyleSheet(lblStyle);
    maxLabel.setAlignment(Qt::AlignCenter);

    bottomLayout.addWidget(&minLabel);
    bottomLayout.addWidget(&meanLabel);
    bottomLayout.addWidget(&maxLabel);
    bottomLayout.addStretch(1);
    bottomLayout.addWidget(&zoomFitBtn);
    bottomLayout.addWidget(&zoom100Btn);
    bottomLayout.addWidget(&currentZoom);

    layout.addWidget(&fitsWidget);
    layout.addWidget(&histWidget);
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
            QString qfname = args.at(1);
            QByteArray qfnameBytes = qfname.toLocal8Bit();
            int size = qfnameBytes.size();
            char *filename = new char[1000];
            for (int i = 0; i < size; i++)
            {
                filename[i] = qfnameBytes.at(i);
            }
            filename[size] = 0;
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
    bool isColor = image->isColor();

    char tmp[500];
    char tmp2[500];
    char tmp3[500];
    int numPoints = 0;
    uint32_t *histogram = 0;
    switch (image->getBitDepth())
    {
    case ELS::FITS::BD_INT_8:
    {
        StatisticsVisitor<uint8_t> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, " min: %d ", visitor.getMinVal());
            sprintf(tmp2, " mean: %d ", visitor.getMeanVal());
            sprintf(tmp3, " max: %d ", visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, " min: %d/%d/%d ",
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, " mean: %d/%d/%d ",
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, " max: %d/%d/%d ",
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        maxLabel.setText(tmp3);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_INT_16:
    {
        StatisticsVisitor<uint16_t> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, " min: %d ", visitor.getMinVal());
            sprintf(tmp2, " mean: %d ", visitor.getMeanVal());
            sprintf(tmp3, " max: %d ", visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, " min: %d/%d/%d ",
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, " mean: %d/%d/%d ",
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, " max: %d/%d/%d ",
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        maxLabel.setText(tmp3);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_INT_32:
    {
        StatisticsVisitor<uint32_t> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, " min: %d ", visitor.getMinVal());
            sprintf(tmp2, " mean: %d ", visitor.getMeanVal());
            sprintf(tmp3, " max: %d ", visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, " min: %d/%d/%d ",
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, " mean: %d/%d/%d ",
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, " max: %d/%d/%d ",
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        maxLabel.setText(tmp3);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_FLOAT:
    {
        StatisticsVisitor<float> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, " min: %0.4f ", visitor.getMinVal());
            sprintf(tmp2, " mean: %0.4f ", visitor.getMeanVal());
            sprintf(tmp3, " max: %0.4f ", visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, " min: %0.4f/%0.4f/%0.4f ",
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, " mean: %0.4f/%0.4f/%0.4f ",
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, " max: %0.4f/%0.4f/%0.4f ",
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        maxLabel.setText(tmp3);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_DOUBLE:
    {
        StatisticsVisitor<double> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, " min: %0.4lf ", visitor.getMinVal());
            sprintf(tmp2, " mean: %0.4f ", visitor.getMeanVal());
            sprintf(tmp2, " max: %0.4lf ", visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, " min: %0.4lf/%0.4lf/%0.4lf ",
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, " mean: %0.4f/%0.4f/%0.4f ",
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, " max: %0.4f/%0.4f/%0.4f ",
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        maxLabel.setText(tmp3);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    }

    histWidget.setHistogramData(isColor, numPoints, histogram);

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
