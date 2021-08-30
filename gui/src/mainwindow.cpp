#include <QApplication>
#include <QDataStream>
#include <QTcpSocket>

#include <memory>

#include "image.h"
#include "mainwindow.h"
#include "statisticsvisitor.h"

MainWindow::MainWindow(QTcpServer& server,
                       QList<ImageFileListItem> fileList,
                       QWidget* parent)
    : QMainWindow(parent),
      server(server),
      clients(),
      fileList(fileList),
      currentFileIdx(0),
      showingStretched(false),
      mainPane(),
      layout(&mainPane),
      imageWidget(),
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
      fileListPosLabel(" -- of -- ")
{
    const QSize iconSize(20, 20);
    const QSize btnSize(30, 30);
    const char* btnStyle = "QPushButton{border: none;border-radius: 7px;background-color: #444;}";
    const char* lblStyle = "QLabel{border: 1px solid #666;border-radius: 7px;color: #999;}";
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

    layout.addWidget(&imageWidget);
    layout.addLayout(&statsHistLayout);
    layout.addLayout(&bottomLayout);

    setCentralWidget(&mainPane);

    // QObject::connect(&imageWidget, &ImageWidget::fileChanged,
    //                  this, &MainWindow::fitsFileChanged);
    // QObject::connect(&imageWidget, &ImageWidget::fileFailed,
    //                  this, &MainWindow::fitsFileFailed);
    QObject::connect(&imageWidget, &ImageWidget::actualZoomChanged,
                     this, &MainWindow::imageZoomChanged);
    QObject::connect(&stretchBtn, &QPushButton::toggled,
                     this, &MainWindow::stretchToggled);
    // QObject::connect(this, &MainWindow::showStretched,
    //                  &imageWidget, &ImageWidget::showStretched);
    // QObject::connect(this, &MainWindow::clearStretched,
    //                  &imageWidget, &ImageWidget::clearStretched);
    QObject::connect(&zoomFitBtn, &QPushButton::clicked,
                     this, &MainWindow::zoomFitClicked);
    QObject::connect(&zoom100Btn, &QPushButton::clicked,
                     this, &MainWindow::zoom100Clicked);
    QObject::connect(&prevBtn, &QPushButton::clicked,
                     this, &MainWindow::prevClicked);
    QObject::connect(&nextBtn, &QPushButton::clicked,
                     this, &MainWindow::nextClicked);
    QObject::connect(&server, &QTcpServer::newConnection,
                     this, &MainWindow::newConnection);
    QObject::connect(&server, &QTcpServer::acceptError,
                     this, &MainWindow::acceptError);

    syncFileIdx();
}

MainWindow::~MainWindow()
{
}

// void MainWindow::fitsFileChanged(const char* filename)
// {
//     printf("File loaded: %s\n", filename);

//     const ELS::Image* image = imageWidget.getImage();

//     minLabel.setText(tmp);
//     meanLabel.setText(tmp2);
//     medLabel.setText(tmp3);
//     maxLabel.setText(tmp4);

//     histWidget.setHistogramData(isColor, numPoints, histogram);
//     imageWidget.setHistogramData(isColor, numPoints, histogram);

//     update();
// }

// void MainWindow::fitsFileFailed(const char* filename,
//                                 const char* errText)
// {
//     printf("File failed [%s]: %s\n", errText, filename);
// }

void MainWindow::imageZoomChanged(float zoom)
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

        syncStretch();

        fileList[currentFileIdx].setShowStretched(showingStretched);
        imageWidget.setImage(fileList[currentFileIdx].getQImage());
    }
}

void MainWindow::zoomFitClicked(bool /* isChecked */)
{
    imageWidget.setZoom(-1.0);
}

void MainWindow::zoom100Clicked(bool /* isChecked */)
{
    imageWidget.setZoom(1.0);
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

void MainWindow::newConnection()
{
    QTcpSocket* sock = server.nextPendingConnection();

    clients.append(sock);

    QObject::connect(sock, &QTcpSocket::disconnected,
                     this, &MainWindow::disconnected);
    QObject::connect(sock, &QIODevice::readyRead,
                     this, &MainWindow::readyRead);
}

void MainWindow::acceptError(QAbstractSocket::SocketError error)
{
    (void)error;
    fprintf(stderr, "Problem accepting tcp connection: %s\n",
            server.errorString().toLocal8Bit().constData());
    fflush(stderr);
}

void MainWindow::readyRead()
{
    QList<QTcpSocket*>::iterator i;
    int idx;
    for (idx = 0, i = clients.begin(); i != clients.end(); ++i, ++idx)
    {
        qint64 bytesAvailable = (*i)->bytesAvailable();

        if (bytesAvailable)
        {
            QDataStream in(*i);

            qint32 numFiles = 0;
            in >> numFiles;

            int newFileCount = 0;
            for (int i = 0; i < numFiles; i++)
            {
                ImageFileListItem item;
                in >> item;
                if (!fileList.contains(item))
                {
                    fileList.append(item);
                    newFileCount++;
                    syncFileCount();
                }
            }

            printf("Added %d files\n", newFileCount);
            fflush(stdout);

            // addFilesToList(absoluteFilePaths);
        }
    }
}

void MainWindow::disconnected()
{
    QList<QTcpSocket*>::iterator i;
    int idx;
    for (idx = 0, i = clients.begin(); i != clients.end(); ++i, ++idx)
    {
        if ((*i)->state() == QAbstractSocket::UnconnectedState)
        {
            QTcpSocket* tmp = *i;
            tmp->close();
            i = clients.erase(i);

            if (i == clients.end())
            {
                break;
            }
        }
    }
}

void MainWindow::syncFileIdx()
{
    ImageFileListItem* item = &(fileList[currentFileIdx]);
    filename = item->absolutePath();

    if (!item->isLoaded())
    {
        printf("Loading %s\n", qPrintable(filename));
        fflush(stdout);
        item->load();
    }

    minLabel.setText(item->getMin());
    meanLabel.setText(item->getMean());
    medLabel.setText(item->getMedian());
    maxLabel.setText(item->getMax());

    histWidget.setHistogramData(item->isColor(),
                                item->getNumHistogramPoints(),
                                item->getHistogram());

    showingStretched = item->showStretched();
    syncStretch();

    syncFileCount();

    printf("Setting file %d of %d: %s\n",
           currentFileIdx + 1,
           fileList.size(),
           qPrintable(filename));
    fflush(stdout);

    imageWidget.setImage(fileList[currentFileIdx].getQImage());
}

void MainWindow::syncFileCount()
{
    char tmp[50];

    sprintf(tmp, " %d of %d ",
            currentFileIdx + 1,
            fileList.size());
    fileListPosLabel.setText(tmp);
}

void MainWindow::syncStretch()
{
    stretchBtn.setChecked(showingStretched);
    if (showingStretched)
    {
        stretchBtn.setIcon(onIcon);
    }
    else
    {
        stretchBtn.setIcon(offIcon);
    }
}

// void MainWindow::addFilesToList(QList<QString> absoluteFilePaths)
// {
//     QList<QString>::iterator i;
//     for (i = absoluteFilePaths.begin(); i != absoluteFilePaths.end(); ++i)
//     {
//         QFileInfo info(*i);

//         if (!fileList.contains(info))
//         {
//             fileList.append(info);
//         }
//     }

//     syncFileCount();
// }
