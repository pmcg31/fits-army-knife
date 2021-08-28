#pragma once

#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QImage& image,
               QWidget* parent = nullptr);
    ~MainWindow();

private:
    QWidget mainPane;
    QVBoxLayout layout;
    QLabel imageLbl;
    QImage image;
};
