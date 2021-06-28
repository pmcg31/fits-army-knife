#include <QApplication>

#include <memory>

#include "fits.h"
#include "fitsimage.h"
#include "statisticsvisitor.h"
#include "mainwindow.h"

MainWindow::MainWindow(QList<QFileInfo> fileList,
                       QWidget *parent)
    : QMainWindow(parent),
      fileList(fileList),
      currentFileIdx(0),
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
      prevBtn(" ◀ "),
      nextBtn(" ▶ "),
      fileListPosLabel(" -- of -- "),
      stfParms()
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
    prevBtn.setEnabled(true);
    prevBtn.setStyleSheet(btnStyle);
    prevBtn.setMinimumSize(btnSize);
    prevBtn.setMaximumSize(btnSize);
    nextBtn.setEnabled(true);
    nextBtn.setStyleSheet(btnStyle);
    nextBtn.setMinimumSize(btnSize);
    nextBtn.setMaximumSize(btnSize);

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

    fileListPosLabel.setStyleSheet(lblStyle);
    fileListPosLabel.setAlignment(Qt::AlignCenter);
    fileListPosLabel.setMinimumHeight(height);
    fileListPosLabel.setMaximumHeight(height);

    bottomLayout.addWidget(&stretchBtn);
    bottomLayout.addStretch(1);
    bottomLayout.addWidget(&prevBtn);
    bottomLayout.addWidget(&fileListPosLabel);
    bottomLayout.addWidget(&nextBtn);
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
    QObject::connect(&prevBtn, &QPushButton::clicked,
                     this, &MainWindow::prevClicked);
    QObject::connect(&nextBtn, &QPushButton::clicked,
                     this, &MainWindow::nextClicked);

    syncFileIdx();
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
    std::shared_ptr<uint32_t[]> histogram;
    switch (image->getBitDepth())
    {
    case ELS::FITS::BD_INT_8:
    {
        ELS::StatisticsVisitor<uint8_t> visitor;
        image->visitPixels(&visitor);
        ELS::PixStatistics<uint8_t> localStats = visitor.getStatistics();
        stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            sprintf(tmp, giMinF, localStats.getMinVal());
            sprintf(tmp2, giMeanF, localStats.getMeanVal());
            sprintf(tmp3, giMedF, localStats.getMedVal());
            sprintf(tmp4, giMaxF, localStats.getMaxVal());
        }
        else
        {
            sprintf(tmp, ciMinF,
                    localStats.getMinVal(0),
                    localStats.getMinVal(1),
                    localStats.getMinVal(2));
            sprintf(tmp2, ciMeanF,
                    localStats.getMeanVal(0),
                    localStats.getMeanVal(1),
                    localStats.getMeanVal(2));
            sprintf(tmp3, ciMedF,
                    localStats.getMedVal(0),
                    localStats.getMedVal(1),
                    localStats.getMedVal(2));
            sprintf(tmp4, ciMaxF,
                    localStats.getMaxVal(0),
                    localStats.getMaxVal(1),
                    localStats.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_INT_16:
    {
        ELS::StatisticsVisitor<uint16_t> visitor;
        image->visitPixels(&visitor);
        ELS::PixStatistics<uint16_t> localStats = visitor.getStatistics();
        stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            sprintf(tmp, giMinF, localStats.getMinVal());
            sprintf(tmp2, giMeanF, localStats.getMeanVal());
            sprintf(tmp3, giMedF, localStats.getMedVal());
            sprintf(tmp4, giMaxF, localStats.getMaxVal());
        }
        else
        {
            sprintf(tmp, ciMinF,
                    localStats.getMinVal(0),
                    localStats.getMinVal(1),
                    localStats.getMinVal(2));
            sprintf(tmp2, ciMeanF,
                    localStats.getMeanVal(0),
                    localStats.getMeanVal(1),
                    localStats.getMeanVal(2));
            sprintf(tmp3, ciMedF,
                    localStats.getMedVal(0),
                    localStats.getMedVal(1),
                    localStats.getMedVal(2));
            sprintf(tmp4, ciMaxF,
                    localStats.getMaxVal(0),
                    localStats.getMaxVal(1),
                    localStats.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_INT_32:
    {
        ELS::StatisticsVisitor<uint32_t> visitor;
        image->visitPixels(&visitor);
        ELS::PixStatistics<uint32_t> localStats = visitor.getStatistics();
        stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            sprintf(tmp, giMinF, localStats.getMinVal());
            sprintf(tmp2, giMeanF, localStats.getMeanVal());
            sprintf(tmp3, giMedF, localStats.getMedVal());
            sprintf(tmp4, giMaxF, localStats.getMaxVal());
        }
        else
        {
            sprintf(tmp, ciMinF,
                    localStats.getMinVal(0),
                    localStats.getMinVal(1),
                    localStats.getMinVal(2));
            sprintf(tmp2, ciMeanF,
                    localStats.getMeanVal(0),
                    localStats.getMeanVal(1),
                    localStats.getMeanVal(2));
            sprintf(tmp3, ciMedF,
                    localStats.getMedVal(0),
                    localStats.getMedVal(1),
                    localStats.getMedVal(2));
            sprintf(tmp4, ciMaxF,
                    localStats.getMaxVal(0),
                    localStats.getMaxVal(1),
                    localStats.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_FLOAT:
    {
        ELS::StatisticsVisitor<float> visitor;
        image->visitPixels(&visitor);
        ELS::PixStatistics<float> localStats = visitor.getStatistics();
        stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            sprintf(tmp, gfMinF, localStats.getMinVal());
            sprintf(tmp2, gfMeanF, localStats.getMeanVal());
            sprintf(tmp3, gfMedF, localStats.getMedVal());
            sprintf(tmp4, gfMaxF, localStats.getMaxVal());
        }
        else
        {
            sprintf(tmp, cfMinF,
                    localStats.getMinVal(0),
                    localStats.getMinVal(1),
                    localStats.getMinVal(2));
            sprintf(tmp2, cfMeanF,
                    localStats.getMeanVal(0),
                    localStats.getMeanVal(1),
                    localStats.getMeanVal(2));
            sprintf(tmp3, cfMedF,
                    localStats.getMedVal(0),
                    localStats.getMedVal(1),
                    localStats.getMedVal(2));
            sprintf(tmp4, cfMaxF,
                    localStats.getMaxVal(0),
                    localStats.getMaxVal(1),
                    localStats.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    case ELS::FITS::BD_DOUBLE:
    {
        ELS::StatisticsVisitor<double> visitor;
        ELS::PixStatistics<double> localStats = visitor.getStatistics();
        stfParms = localStats.getStretchParameters();
        image->visitPixels(&visitor);
        if (!isColor)
        {
            sprintf(tmp, gfMinF, localStats.getMinVal());
            sprintf(tmp2, gfMeanF, localStats.getMeanVal());
            sprintf(tmp3, gfMedF, localStats.getMedVal());
            sprintf(tmp4, gfMaxF, localStats.getMaxVal());
        }
        else
        {
            sprintf(tmp, cfMinF,
                    localStats.getMinVal(0),
                    localStats.getMinVal(1),
                    localStats.getMinVal(2));
            sprintf(tmp2, cfMeanF,
                    localStats.getMeanVal(0),
                    localStats.getMeanVal(1),
                    localStats.getMeanVal(2));
            sprintf(tmp3, cfMedF,
                    localStats.getMedVal(0),
                    localStats.getMedVal(1),
                    localStats.getMedVal(2));
            sprintf(tmp4, cfMaxF,
                    localStats.getMaxVal(0),
                    localStats.getMaxVal(1),
                    localStats.getMaxVal(2));
        }
        minLabel.setText(tmp);
        meanLabel.setText(tmp2);
        medLabel.setText(tmp3);
        maxLabel.setText(tmp4);

        visitor.getHistogramData(&numPoints, &histogram);
    }
    break;
    }

    histWidget.setHistogramData(isColor, numPoints, histogram);
    fitsWidget.setHistogramData(isColor, numPoints, histogram);

    printf("%s\n", image->getImageType());
    printf("%s\n", image->getSizeAndColor());
    fflush(stdout);

    update();
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
            emit showStretched(stfParms);
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

void MainWindow::prevClicked(bool isChecked)
{
    (void)isChecked;

    if (currentFileIdx > 0)
    {
        currentFileIdx--;
        syncFileIdx();
    }
}

void MainWindow::nextClicked(bool isChecked)
{
    (void)isChecked;

    if ((currentFileIdx + 1) < fileList.size())
    {
        currentFileIdx++;
        syncFileIdx();
    }
}

void MainWindow::syncFileIdx()
{
    char tmp[50];

    const char *filename = fileList.at(currentFileIdx)
                               .absoluteFilePath()
                               .toLocal8Bit()
                               .constData();

    sprintf(tmp, " %d of %d ",
            currentFileIdx + 1,
            fileList.size());
    fileListPosLabel.setText(tmp);

    printf("Setting file %d of %d: %s\n",
           currentFileIdx + 1,
           fileList.size(),
           filename);
    fflush(stdout);

    fitsWidget.setFile(filename);
}