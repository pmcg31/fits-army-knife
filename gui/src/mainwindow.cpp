#include <QApplication>

#include "mainwindow.h"
#include "fits.h"
#include "fitsimage.h"
#include "statisticsvisitor.h"
#include "adaptivedisplayfuncvisitor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      filename(""),
      showingStretched(false),
      mainPane(),
      layout(&mainPane),
      fitsWidget(),
      histWidget(),
      bottomLayout(),
      statsHistLayout(),
      statsLayout(),
      minLabel(" min: -- "),
      meanLabel(" mean: -- "),
      medLabel(" med: -- "),
      maxLabel(" max: -- "),
      currentZoom("--"),
      onIcon(":/icon/stretch-icon.png"),
      offIcon(":/icon/stretch-icon-off.png"),
      stretchBtn(offIcon, ""),
      zoomFitBtn("fit"),
      zoom100Btn("1:1"),
      madn{0.0, 0.0, 0.0},
      sClip{0.0, 0.0, 0.0},
      hClip{1.0, 1.0, 1.0},
      mBal{0.5, 0.5, 0.5},
      sExp{0.0, 0.0, 0.0},
      hExp{1.0, 1.0, 1.0}
{
    const QSize iconSize(20, 20);
    const QSize btnSize(30, 30);
    const char *btnStyle = "QPushButton{border: none;border-radius: 7px;background-color: #444;}";
    const char *lblStyle = "QLabel{border: 1px solid #666;border-radius: 7px;color: #999;}";
    stretchBtn.setStyleSheet(btnStyle);
    stretchBtn.setIconSize(iconSize);
    stretchBtn.setMinimumSize(btnSize);
    stretchBtn.setMaximumSize(btnSize);
    stretchBtn.setCheckable(true);
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
    QSize hint = currentZoom.sizeHint();
    int height = hint.height() * 1.5;
    currentZoom.setMinimumHeight(height);
    currentZoom.setMaximumHeight(height);

    minLabel.setStyleSheet(lblStyle);
    minLabel.setAlignment(Qt::AlignCenter);
    minLabel.setMinimumHeight(height);
    minLabel.setMaximumHeight(height);
    meanLabel.setStyleSheet(lblStyle);
    meanLabel.setAlignment(Qt::AlignCenter);
    meanLabel.setMinimumHeight(height);
    meanLabel.setMaximumHeight(height);
    medLabel.setStyleSheet(lblStyle);
    medLabel.setAlignment(Qt::AlignCenter);
    medLabel.setMinimumHeight(height);
    medLabel.setMaximumHeight(height);
    maxLabel.setStyleSheet(lblStyle);
    maxLabel.setAlignment(Qt::AlignCenter);
    maxLabel.setMinimumHeight(height);
    maxLabel.setMaximumHeight(height);

    bottomLayout.addWidget(&stretchBtn);
    bottomLayout.addStretch(1);
    bottomLayout.addWidget(&zoomFitBtn);
    bottomLayout.addWidget(&zoom100Btn);
    bottomLayout.addWidget(&currentZoom);

    statsLayout.addWidget(&minLabel);
    statsLayout.addWidget(&meanLabel);
    statsLayout.addWidget(&medLabel);
    statsLayout.addWidget(&maxLabel);

    statsHistLayout.addLayout(&statsLayout);
    statsHistLayout.addWidget(&histWidget);

    layout.addWidget(&fitsWidget);
    layout.addLayout(&statsHistLayout);
    layout.addLayout(&bottomLayout);

    setCentralWidget(&mainPane);

    QObject::connect(&fitsWidget, &FITSWidget::fileChanged,
                     this, &MainWindow::fitsFileChanged);
    QObject::connect(&fitsWidget, &FITSWidget::fileFailed,
                     this, &MainWindow::fitsFileFailed);
    QObject::connect(&fitsWidget, &FITSWidget::actualZoomChanged,
                     this, &MainWindow::fitsZoomChanged);
    QObject::connect(&stretchBtn, &QPushButton::toggled,
                     this, &MainWindow::stretchToggled);
    QObject::connect(this, &MainWindow::showStretched,
                     &fitsWidget, &FITSWidget::showStretched);
    QObject::connect(this, &MainWindow::clearStretched,
                     &fitsWidget, &FITSWidget::clearStretched);
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

    const char *giMinF = " min: %d ";
    const char *giMeanF = " mean: %d ";
    const char *giMedF = " median: %d ";
    const char *giMaxF = " max: %d ";
    const char *ciMinF = " min: %d | %d | %d";
    const char *ciMeanF = " mean: %d | %d | %d";
    const char *ciMedF = " median: %d | %d | %d ";
    const char *ciMaxF = " max: %d | %d | %d ";
    const char *gfMinF = " min: %d0.4f ";
    const char *gfMeanF = " mean: %0.4f ";
    const char *gfMedF = " median: %0.4f ";
    const char *gfMaxF = " max: %0.4f ";
    const char *cfMinF = " min: %0.4f | %0.4f | %0.4f";
    const char *cfMeanF = " mean: %0.4f | %0.4f | %0.4f";
    const char *cfMedF = " median: %0.4f | %0.4f | %0.4f ";
    const char *cfMaxF = " max: %0.4f | %0.4f | %0.4f ";

    char tmp[100];
    char tmp2[100];
    char tmp3[100];
    char tmp4[100];
    int numPoints = 0;
    uint32_t *histogram = 0;
    switch (image->getBitDepth())
    {
    case ELS::FITS::BD_INT_8:
    {
        ELS::StatisticsVisitor<uint8_t> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, giMinF, visitor.getMinVal());
            sprintf(tmp2, giMeanF, visitor.getMeanVal());
            sprintf(tmp3, giMedF, visitor.getMedVal());
            sprintf(tmp4, giMaxF, visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, ciMinF,
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, ciMeanF,
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, ciMedF,
                    visitor.getMedVal(0),
                    visitor.getMedVal(1),
                    visitor.getMedVal(2));
            sprintf(tmp4, ciMaxF,
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);

        uint8_t medVals[3] = {visitor.getMedVal(0),
                              visitor.getMedVal(1),
                              visitor.getMedVal(2)};
        ELS::AdaptiveDisplayFuncVisitor<uint8_t> v2(medVals);
        image->visitPixels(&v2);
        v2.getMADN(madn);
        v2.getSClip(sClip);
        v2.getHClip(hClip);
        v2.getMBal(mBal);
    }
    break;
    case ELS::FITS::BD_INT_16:
    {
        ELS::StatisticsVisitor<uint16_t> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, giMinF, visitor.getMinVal());
            sprintf(tmp2, giMeanF, visitor.getMeanVal());
            sprintf(tmp3, giMedF, visitor.getMedVal());
            sprintf(tmp4, giMaxF, visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, ciMinF,
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, ciMeanF,
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, ciMedF,
                    visitor.getMedVal(0),
                    visitor.getMedVal(1),
                    visitor.getMedVal(2));
            sprintf(tmp4, ciMaxF,
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);

        uint16_t medVals[3] = {visitor.getMedVal(0),
                               visitor.getMedVal(1),
                               visitor.getMedVal(2)};
        ELS::AdaptiveDisplayFuncVisitor<uint16_t> v2(medVals);
        image->visitPixels(&v2);
        v2.getMADN(madn);
        v2.getSClip(sClip);
        v2.getHClip(hClip);
        v2.getMBal(mBal);
    }
    break;
    case ELS::FITS::BD_INT_32:
    {
        ELS::StatisticsVisitor<uint32_t> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, giMinF, visitor.getMinVal());
            sprintf(tmp2, giMeanF, visitor.getMeanVal());
            sprintf(tmp3, giMedF, visitor.getMedVal());
            sprintf(tmp4, giMaxF, visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, ciMinF,
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, ciMeanF,
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, ciMedF,
                    visitor.getMedVal(0),
                    visitor.getMedVal(1),
                    visitor.getMedVal(2));
            sprintf(tmp4, ciMaxF,
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);

        uint32_t medVals[3] = {visitor.getMedVal(0),
                               visitor.getMedVal(1),
                               visitor.getMedVal(2)};
        ELS::AdaptiveDisplayFuncVisitor<uint32_t> v2(medVals);
        image->visitPixels(&v2);
        v2.getMADN(madn);
        v2.getSClip(sClip);
        v2.getHClip(hClip);
        v2.getMBal(mBal);
    }
    break;
    case ELS::FITS::BD_FLOAT:
    {
        ELS::StatisticsVisitor<float> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, gfMinF, visitor.getMinVal());
            sprintf(tmp2, gfMeanF, visitor.getMeanVal());
            sprintf(tmp3, gfMedF, visitor.getMedVal());
            sprintf(tmp4, gfMaxF, visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, cfMinF,
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, cfMeanF,
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, cfMedF,
                    visitor.getMedVal(0),
                    visitor.getMedVal(1),
                    visitor.getMedVal(2));
            sprintf(tmp4, cfMaxF,
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);

        float medVals[3] = {visitor.getMedVal(0),
                            visitor.getMedVal(1),
                            visitor.getMedVal(2)};
        ELS::AdaptiveDisplayFuncVisitor<float> v2(medVals);
        image->visitPixels(&v2);
        v2.getMADN(madn);
        v2.getSClip(sClip);
        v2.getHClip(hClip);
        v2.getMBal(mBal);
    }
    break;
    case ELS::FITS::BD_DOUBLE:
    {
        ELS::StatisticsVisitor<double> visitor;
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, gfMinF, visitor.getMinVal());
            sprintf(tmp2, gfMeanF, visitor.getMeanVal());
            sprintf(tmp3, gfMedF, visitor.getMedVal());
            sprintf(tmp4, gfMaxF, visitor.getMaxVal());
        }
        else
        {
            sprintf(tmp, cfMinF,
                    visitor.getMinVal(0),
                    visitor.getMinVal(1),
                    visitor.getMinVal(2));
            sprintf(tmp2, cfMeanF,
                    visitor.getMeanVal(0),
                    visitor.getMeanVal(1),
                    visitor.getMeanVal(2));
            sprintf(tmp3, cfMedF,
                    visitor.getMedVal(0),
                    visitor.getMedVal(1),
                    visitor.getMedVal(2));
            sprintf(tmp4, cfMaxF,
                    visitor.getMaxVal(0),
                    visitor.getMaxVal(1),
                    visitor.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);

        double medVals[3] = {visitor.getMedVal(0),
                             visitor.getMedVal(1),
                             visitor.getMedVal(2)};
        ELS::AdaptiveDisplayFuncVisitor<double> v2(medVals);
        image->visitPixels(&v2);
        v2.getMADN(madn);
        v2.getSClip(sClip);
        v2.getHClip(hClip);
        v2.getMBal(mBal);
    }
    break;
    }

    histWidget.setHistogramData(isColor, numPoints, histogram);

    printf("%s\n", image->getImageType());
    printf("%s\n", image->getSizeAndColor());
    int numChan = image->isColor() ? 3 : 1;
    for (int chan = 0; chan < numChan; chan++)
    {
        printf("c%d: madn: %0.4f sClip: %0.4f hClip: %0.4f mBal: %0.4f\n",
               chan, madn[chan], sClip[chan], hClip[chan], mBal[chan]);
    }
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

void MainWindow::stretchToggled(bool isChecked)
{
    if (showingStretched != isChecked)
    {
        showingStretched = isChecked;

        stretchBtn.setChecked(showingStretched);
        if (showingStretched)
        {
            stretchBtn.setIcon(onIcon);
            emit showStretched(mBal, sClip, hClip, sExp, hExp);
        }
        else
        {
            stretchBtn.setIcon(offIcon);
            emit clearStretched();
        }
    }
}

void MainWindow::zoomFitClicked(bool /* isChecked */)
{
    fitsWidget.setZoom(-1.0);
}

void MainWindow::zoom100Clicked(bool /* isChecked */)
{
    fitsWidget.setZoom(1.0);
}
