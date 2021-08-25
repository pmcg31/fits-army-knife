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
#include "pixelvisitor.h"
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

    void convertImage();

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
    class ToQImageVisitor : public ELS::PixelVisitor<PixelT>
    {
    public:
        ToQImageVisitor(ELS::PixSTFParms stfParms,
                        uint8_t *lut,
                        int lutPoints);
        ~ToQImageVisitor();

        std::shared_ptr<uint32_t[]> getImageData();
        std::shared_ptr<QImage> getImage();

    public:
        virtual void pixelFormat(ELS::PixelFormat pf) override;
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
        int _width;
        int _height;
        int _pixCount;
        std::shared_ptr<uint32_t[]> _qiData;
        int _stride;
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
    std::shared_ptr<uint32_t[]> _cacheImageData;
    bool _showStretched;
    float _zoom;
    float _actualZoom;
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
    : _width(0),
      _height(0),
      _pixCount(0),
      _qiData(),
      _stride(0),
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
std::shared_ptr<uint32_t[]> FITSWidget::ToQImageVisitor<PixelT>::getImageData()
{
    return _qiData;
}

template <typename PixelT>
std::shared_ptr<QImage> FITSWidget::ToQImageVisitor<PixelT>::getImage()
{
    return _qi;
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::pixelFormat(ELS::PixelFormat pf)
{
    (void)pf;
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::dimensions(int width,
                                                     int height)
{
    _width = width;
    _height = height;
    _pixCount = _width * _height;

    _qiData.reset(new uint32_t[_pixCount]);
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
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::rowRgb(int y,
                                                 const PixelT *r,
                                                 const PixelT *g,
                                                 const PixelT *b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::done()
{
    _qi.reset(new QImage((const uchar *)_qiData.get(), _width, _height, QImage::Format_ARGB32));
}

#endif // FITSWIDGET_H
