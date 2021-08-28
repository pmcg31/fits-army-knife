#ifndef FITSWIDGET_H
#define FITSWIDGET_H

#include <memory>

#include <QSizePolicy>
#include <QString>
#include <QWheelEvent>
#include <QWidget>
#include <fitsio.h>

#include "fitsimage.h"
#include "pixelvisitor.h"
#include "pixstatistics.h"
#include "pixstfparms.h"
#include "pixutils.h"

class FITSWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FITSWidget(QWidget* parent = nullptr);
    ~FITSWidget();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    const ELS::FITSImage* getImage() const;

    const char* getFilename() const;
    bool getStretched() const;
    float getZoom() const;

public slots:
    void setFile(const char* filename);
    void showStretched(ELS::PixSTFParms stfParams);
    void clearStretched();
    void setZoom(float zoom);
    void setHistogramData(bool isColor,
                          int numPoints,
                          std::shared_ptr<uint32_t[]> data);

signals:
    void fileChanged(const char* filename);
    void fileFailed(const char* filename,
                    const char* errText);
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

private:
    void calculateStfLUT();

private:
    class ToQImageVisitor : public ELS::PixelVisitor
    {
    public:
        ToQImageVisitor(ELS::PixSTFParms stfParms,
                        uint8_t* lut,
                        int lutPoints);
        ~ToQImageVisitor();

        std::shared_ptr<uint32_t[]> getImageData();
        std::shared_ptr<QImage> getImage();

    public:
        virtual void pixelFormat(ELS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void rowInfo(int stride) override;

        virtual void rowGray(int y,
                             const int8_t* k) override;
        virtual void rowGray(int y,
                             const int16_t* k) override;
        virtual void rowGray(int y,
                             const int32_t* k) override;
        virtual void rowGray(int y,
                             const uint8_t* k) override;
        virtual void rowGray(int y,
                             const uint16_t* k) override;
        virtual void rowGray(int y,
                             const uint32_t* k) override;
        virtual void rowGray(int y,
                             const float* k) override;
        virtual void rowGray(int y,
                             const double* k) override;

        virtual void rowRgb(int y,
                            const int8_t* r,
                            const int8_t* g,
                            const int8_t* b) override;
        virtual void rowRgb(int y,
                            const int16_t* r,
                            const int16_t* g,
                            const int16_t* b) override;
        virtual void rowRgb(int y,
                            const int32_t* r,
                            const int32_t* g,
                            const int32_t* b) override;
        virtual void rowRgb(int y,
                            const uint8_t* r,
                            const uint8_t* g,
                            const uint8_t* b) override;
        virtual void rowRgb(int y,
                            const uint16_t* r,
                            const uint16_t* g,
                            const uint16_t* b) override;
        virtual void rowRgb(int y,
                            const uint32_t* r,
                            const uint32_t* g,
                            const uint32_t* b) override;
        virtual void rowRgb(int y,
                            const float* r,
                            const float* g,
                            const float* b) override;
        virtual void rowRgb(int y,
                            const double* r,
                            const double* g,
                            const double* b) override;

        virtual void done() override;

    private:
        int _width;
        int _height;
        int _pixCount;
        std::shared_ptr<uint32_t[]> _qiData;
        int _stride;
        ELS::PixSTFParms _stfParms;
        std::shared_ptr<QImage> _qi;
        uint8_t* _lut;
        int _lutPoints;
    };

private:
    QSizePolicy _sizePolicy;
    char _filename[500];
    ELS::FITSImage* _fits;
    std::shared_ptr<QImage> _cacheImage;
    std::shared_ptr<uint32_t[]> _cacheImageData;
    bool _showStretched;
    float _zoom;
    float _actualZoom;
    ELS::PixSTFParms _stfParms;
    bool _isColor;
    int _numHistogramPoints;
    std::shared_ptr<uint32_t[]> _histogramData;
    uint8_t* _stfLUT;
    uint8_t* _identityLUT;
    uint8_t* _lutInUse;
    QPoint _mouseDragLast;
    QRect _source;
    QRect _target;
    QPoint _imageZoomLockPoint;
    QPoint _windowZoomLockPoint;

private:
    static const float g_validZooms[];
};

#endif // FITSWIDGET_H
