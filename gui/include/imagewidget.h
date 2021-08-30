#pragma once

#include <memory>

#include <QSizePolicy>
#include <QString>
#include <QWheelEvent>
#include <QWidget>
#include <fitsio.h>

#include "image.h"
#include "pixelvisitor.h"
#include "pixstatistics.h"
#include "pixstfparms.h"
#include "pixutils.h"

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget* parent = nullptr);
    ~ImageWidget();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // const ELS::Image* getImage() const;

    // const char* getFilename() const;
    // bool getStretched() const;
    float getZoom() const;

public slots:
    void setImage(std::shared_ptr<const QImage> image);
    // void setFile(const char* filename);
    // void showStretched();
    // void clearStretched();
    void setZoom(float zoom);
    // void setHistogramData(bool isColor,
    //                       int numPoints,
    //                       std::shared_ptr<uint32_t[]> data);

signals:
    // void fileChanged(const char* filename);
    // void fileFailed(const char* filename,
    //                 const char* errText);
    void zoomChanged(float zoom);
    void actualZoomChanged(float zoom);

protected:
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

protected:
    enum ZoomAdjustStrategy
    {
        ZAS_CLOSEST,
        ZAS_HIGHER,
        ZAS_LOWER
    };

    void _internalSetZoom(float zoom);

    static float adjustZoom(float desiredZoom,
                            ZoomAdjustStrategy strategy = ZAS_CLOSEST);

    // private:
    //     void calculateStfLUT();

private:
    QSizePolicy _sizePolicy;
    char _filename[500];
    // ELS::Image* _image;
    std::shared_ptr<const QImage> _image;
    // std::shared_ptr<uint32_t[]> _cacheImageData;
    // bool _showStretched;
    float _zoom;
    float _actualZoom;
    // ELS::PixSTFParms _stfParms;
    // bool _isColor;
    // int _numHistogramPoints;
    // std::shared_ptr<uint32_t[]> _histogramData;
    // uint8_t* _stfLUT;
    // uint8_t* _identityLUT;
    // uint8_t* _lutInUse;
    QPoint _mouseDragLast;
    QRect _source;
    QRect _target;
    QPoint _imageZoomLockPoint;
    QPoint _windowZoomLockPoint;

private:
    static const float g_validZooms[];
};
