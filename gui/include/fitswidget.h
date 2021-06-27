#ifndef FITSWIDGET_H
#define FITSWIDGET_H

#include <memory>

#include <QWidget>
#include <QSizePolicy>
#include <QWheelEvent>
#include <QString>
#include <fitsio.h>

#include "fits.h"
#include "fitsimage.h"
#include "fitspixelvisitor.h"
#include "pixutils.h"
#include "pixstatistics.h"
#include "pixstfparms.h"

class FITSWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FITSWidget(QWidget *parent = nullptr);
    ~FITSWidget();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    const ELS::FITSImage *getImage() const;

    const char *getFilename() const;
    bool getStretched() const;
    float getZoom() const;

public slots:
    void setFile(const char *filename);
    void showStretched(ELS::PixSTFParms stfParams);
    void clearStretched();
    void setZoom(float zoom);
    void setHistogramData(bool isColor,
                          int numPoints,
                          std::shared_ptr<uint32_t[]> data);

signals:
    void fileChanged(const char *filename);
    void fileFailed(const char *filename,
                    const char *errText);
    void zoomChanged(float zoom);
    void actualZoomChanged(float zoom);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    std::shared_ptr<QImage> convertImage() const;

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
    template <typename PixelT>
    class ToQImageVisitor : public ELS::FITSPixelVisitor<PixelT>
    {
    public:
        ToQImageVisitor(ELS::PixSTFParms stfParms,
                        uint8_t *lut,
                        int lutPoints);
        ~ToQImageVisitor();

        std::shared_ptr<QImage> getImage();

    public:
        virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void rowInfo(int stride) override;
        virtual void rowGray(int y,
                             const PixelT *k) override;
        virtual void rowRgb(int y,
                            const PixelT *r,
                            const PixelT *g,
                            const PixelT *b) override;
        virtual void done() override;

    private:
        bool _isColor;
        int _width;
        int _stride;
        int _rIdx;
        int _gIdx;
        int _bIdx;
        ELS::PixSTFParms _stfParms;
        std::shared_ptr<QImage> _qi;
        uint8_t *_lut;
        int _lutPoints;
    };

private:
    QSizePolicy _sizePolicy;
    char _filename[500];
    ELS::FITSImage *_fits;
    std::shared_ptr<QImage> _cacheImage;
    bool _showStretched;
    float _zoom;
    float _actualZoom;
    QPoint _centerPoint;
    ELS::PixSTFParms _stfParms;
    bool _isColor;
    int _numHistogramPoints;
    std::shared_ptr<uint32_t[]> _histogramData;
    uint8_t *_stfLUT;
    uint8_t *_identityLUT;
    uint8_t *_lutInUse;
    QPoint _mouseDragLast;
    QRect _source;
    QRect _target;
    QPoint _imageZoomLockPoint;
    QPoint _windowZoomLockPoint;

private:
    static const float g_validZooms[];
};

template <typename PixelT>
FITSWidget::ToQImageVisitor<PixelT>::ToQImageVisitor(ELS::PixSTFParms stfParms,
                                                     uint8_t *lut,
                                                     int lutPoints)
    : _isColor(false),
      _width(0),
      _stride(0),
      _rIdx(0),
      _gIdx(-1),
      _bIdx(-1),
      _stfParms(stfParms),
      _qi(),
      _lut(lut),
      _lutPoints(lutPoints)
{
}

template <typename PixelT>
FITSWidget::ToQImageVisitor<PixelT>::~ToQImageVisitor()
{
}

template <typename PixelT>
std::shared_ptr<QImage> FITSWidget::ToQImageVisitor<PixelT>::getImage()
{
    return _qi;
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::pixelFormat(ELS::FITS::PixelFormat pf)
{
    _isColor = true;
    switch (pf)
    {
    case ELS::FITS::PF_GRAY:
        _isColor = false;
        _rIdx = 0;
        _gIdx = -1;
        _bIdx = -1;
        break;
    case ELS::FITS::PF_RGB:
        _rIdx = 0;
        _gIdx = 1;
        _bIdx = 2;
        break;
    case ELS::FITS::PF_BGR:
        _rIdx = 2;
        _gIdx = 1;
        _bIdx = 0;
        break;
    }
}
template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::dimensions(int width,
                                                     int height)
{
    _width = width;
    _qi.reset(new QImage(width, height, QImage::Format_ARGB32));
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::rowInfo(int stride)
{
    _stride = stride;
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::rowGray(int y,
                                                  const PixelT *k)
{
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qi->setPixelColor(x, y, QColor::fromRgb(val, val, val));
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::rowRgb(int y,
                                                 const PixelT *r,
                                                 const PixelT *g,
                                                 const PixelT *b)
{
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qi->setPixelColor(x, y, QColor::fromRgb(red, green, blue));
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::done()
{
    // Cool
}

#endif // FITSWIDGET_H
