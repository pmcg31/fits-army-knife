#ifndef FITSWIDGET_H
#define FITSWIDGET_H

#include <QWidget>
#include <QSizePolicy>
#include <QWheelEvent>
#include <QString>
#include <fitsio.h>

#include "fits.h"
#include "fitsimage.h"
#include "fitspixelvisitor.h"
#include "pixutils.h"

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
    void showStretched(const double *mBal,
                       const double *sClip,
                       const double *hClip,
                       const double *sExp,
                       const double *hExp);
    void clearStretched();
    void setZoom(float zoom);

signals:
    void fileChanged(const char *filename);
    void fileFailed(const char *filename,
                    const char *errText);
    void zoomChanged(float zoom);
    void actualZoomChanged(float zoom);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    QImage *convertImage() const;

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
    template <typename PixelT>
    class ToQImageVisitor : public ELS::FITSPixelVisitor<PixelT>
    {
    public:
        ToQImageVisitor();
        ~ToQImageVisitor();

        void applySTF(const double *mBal,
                      const double *sClip,
                      const double *hClip,
                      const double *sExp,
                      const double *hExp);
        void clearSTF();

        QImage *getImage();

    public:
        virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void pixel(int x, int y, const PixelT *pixelVals) override;
        virtual void done() override;

    private:
        QColor convertGrayPixel(uint8_t val);
        QColor convertGrayPixel(uint16_t val);
        QColor convertGrayPixel(uint32_t val);
        QColor convertGrayPixel(float val);
        QColor convertGrayPixel(double val);
        QColor convertColorPixel(const uint8_t *vals);
        QColor convertColorPixel(const uint16_t *vals);
        QColor convertColorPixel(const uint32_t *vals);
        QColor convertColorPixel(const float *vals);
        QColor convertColorPixel(const double *vals);

    private:
        bool _isColor;
        int _rIdx;
        int _gIdx;
        int _bIdx;
        double _mBal[3];
        double _sClip[3];
        double _hClip[3];
        double _sExp[3];
        double _hExp[3];
        QImage *_qi;
        bool _shouldDeleteImage;
    };

private:
    QSizePolicy _sizePolicy;
    char _filename[500];
    ELS::FITSImage *_fits;
    QImage *_cacheImage;
    bool _showStretched;
    float _zoom;
    float _actualZoom;
    double _mBal[3];
    double _sClip[3];
    double _hClip[3];
    double _sExp[3];
    double _hExp[3];

private:
    static const float g_validZooms[];
};

template <typename PixelT>
FITSWidget::ToQImageVisitor<PixelT>::ToQImageVisitor()
    : _isColor(false),
      _rIdx(0),
      _gIdx(-1),
      _bIdx(-1),
      _mBal{0.5, 0.5, 0.5},
      _sClip{0.0, 0.0, 0.0},
      _hClip{1.0, 1.0, 1.0},
      _sExp{0.0, 0.0, 0.0},
      _hExp{1.0, 1.0, 1.0},
      _qi(0),
      _shouldDeleteImage(true)
{
}

template <typename PixelT>
FITSWidget::ToQImageVisitor<PixelT>::~ToQImageVisitor()
{
    if (_shouldDeleteImage && (_qi != 0))
    {
        delete _qi;
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::applySTF(const double *mBal,
                                                   const double *sClip,
                                                   const double *hClip,
                                                   const double *sExp,
                                                   const double *hExp)
{
    for (int chan = 0; chan < 3; chan++)
    {
        _mBal[chan] = mBal[chan];
        _sClip[chan] = sClip[chan];
        _hClip[chan] = hClip[chan];
        _sExp[chan] = sExp[chan];
        _hExp[chan] = hExp[chan];
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::clearSTF()
{
    for (int chan = 0; chan < 3; chan++)
    {
        _mBal[chan] = 0.5;
        _sClip[chan] = 0.0;
        _hClip[chan] = 1.0;
        _sExp[chan] = 0.0;
        _hExp[chan] = 1.0;
    }
}

template <typename PixelT>
QImage *FITSWidget::ToQImageVisitor<PixelT>::getImage()
{
    _shouldDeleteImage = false;
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
    if (_shouldDeleteImage && (_qi != 0))
    {
        delete _qi;
    }

    _qi = new QImage(width, height, QImage::Format_ARGB32);
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::pixel(int x,
                                                int y,
                                                const PixelT *pixelVals)
{
    if (_isColor)
    {
        _qi->setPixelColor(x, y, convertColorPixel(pixelVals));
    }
    else
    {
        _qi->setPixelColor(x, y, convertGrayPixel(pixelVals[0]));
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::done()
{
    // Cool
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(uint8_t val)
{
    double dval = (double)val / 255.0;
    dval = ELS::PixUtils::screenTransferFunc(dval,
                                             _mBal[0],
                                             _sClip[0],
                                             _hClip[0],
                                             _sExp[0],
                                             _hExp[0]);
    uint8_t newVal = (uint8_t)(dval * 255.0);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(uint16_t val)
{
    double dval = (double)val / 65535.0;
    dval = ELS::PixUtils::screenTransferFunc(dval,
                                             _mBal[0],
                                             _sClip[0],
                                             _hClip[0],
                                             _sExp[0],
                                             _hExp[0]);
    uint8_t newVal = (uint8_t)(dval * 255.0);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(uint32_t val)
{
    double dval = (double)val / 4294967295.0;
    dval = ELS::PixUtils::screenTransferFunc(dval,
                                             _mBal[0],
                                             _sClip[0],
                                             _hClip[0],
                                             _sExp[0],
                                             _hExp[0]);
    uint8_t newVal = (uint8_t)(dval * 255.0);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(float val)
{
    double dval = ELS::PixUtils::screenTransferFunc(val,
                                                    _mBal[0],
                                                    _sClip[0],
                                                    _hClip[0],
                                                    _sExp[0],
                                                    _hExp[0]);
    uint8_t newVal = (uint8_t)(dval * 255.0);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(double val)
{
    double dval = ELS::PixUtils::screenTransferFunc(val,
                                                    _mBal[0],
                                                    _sClip[0],
                                                    _hClip[0],
                                                    _sExp[0],
                                                    _hExp[0]);
    uint8_t newVal = (uint8_t)(dval * 255.0);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const uint8_t *vals)
{
    uint8_t newVals[3];
    int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
    for (int chan = 0; chan < 3; chan++)
    {
        double dval = (double)vals[chanIdx[chan]] / 255.0;
        dval = ELS::PixUtils::screenTransferFunc(dval,
                                                 _mBal[chan],
                                                 _sClip[chan],
                                                 _hClip[chan],
                                                 _sExp[chan],
                                                 _hExp[chan]);
        newVals[chan] = (uint8_t)(dval * 255.0);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const uint16_t *vals)
{
    uint8_t newVals[3];
    int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
    for (int chan = 0; chan < 3; chan++)
    {
        double dval = (double)vals[chanIdx[chan]] / 65535.0;
        dval = ELS::PixUtils::screenTransferFunc(dval,
                                                 _mBal[chan],
                                                 _sClip[chan],
                                                 _hClip[chan],
                                                 _sExp[chan],
                                                 _hExp[chan]);
        newVals[chan] = (uint8_t)(dval * 255.0);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const uint32_t *vals)
{
    uint8_t newVals[3];
    int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
    for (int chan = 0; chan < 3; chan++)
    {
        double dval = (double)vals[chanIdx[chan]] / 4294967295.0;
        dval = ELS::PixUtils::screenTransferFunc(dval,
                                                 _mBal[chan],
                                                 _sClip[chan],
                                                 _hClip[chan],
                                                 _sExp[chan],
                                                 _hExp[chan]);
        newVals[chan] = (uint8_t)(dval * 255.0);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const float *vals)
{
    uint8_t newVals[3];
    int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
    for (int chan = 0; chan < 3; chan++)
    {
        double dval = ELS::PixUtils::screenTransferFunc(vals[chanIdx[chan]],
                                                        _mBal[chan],
                                                        _sClip[chan],
                                                        _hClip[chan],
                                                        _sExp[chan],
                                                        _hExp[chan]);
        newVals[chan] = (uint8_t)(dval * 255.0);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const double *vals)
{
    uint8_t newVals[3];
    int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
    for (int chan = 0; chan < 3; chan++)
    {
        double dval = ELS::PixUtils::screenTransferFunc(vals[chanIdx[chan]],
                                                        _mBal[chan],
                                                        _sClip[chan],
                                                        _hClip[chan],
                                                        _sExp[chan],
                                                        _hExp[chan]);
        newVals[chan] = (uint8_t)(dval * 255.0);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

#endif // FITSWIDGET_H
