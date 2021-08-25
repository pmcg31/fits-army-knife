#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QImage>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QImage &image,
               QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget mainPane;
    QVBoxLayout layout;
    QLabel imageLbl;
    QImage image;
};
