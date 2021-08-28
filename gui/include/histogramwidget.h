#pragma once

#include <QImage>
#include <QWidget>

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget* parent = nullptr);
    ~HistogramWidget();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void setHistogramData(bool isColor,
                          int numPoints,
                          std::shared_ptr<uint32_t[]> data);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    bool _isColor;
    int _numPoints;
    std::shared_ptr<uint32_t[]> _data;
    QImage* _histImage;
};