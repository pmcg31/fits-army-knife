
#include "mainwindow.h"

MainWindow::MainWindow(QImage &image,
                       QWidget *parent)
    : QMainWindow(parent),
      mainPane(),
      layout(&mainPane),
      imageLbl(),
      image(image)
{
    imageLbl.setBackgroundRole(QPalette::Base);
    imageLbl.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLbl.setScaledContents(true);

    imageLbl.setPixmap(QPixmap::fromImage(image));

    layout.addWidget(&imageLbl);

    setCentralWidget(&mainPane);
}

MainWindow::~MainWindow()
{
}
