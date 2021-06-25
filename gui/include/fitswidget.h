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
        ToQImageVisitor(ELS::PixSTFParms stfParms);
        ~ToQImageVisitor();

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
        ELS::PixSTFParms _stfParms;
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
    ELS::PixSTFParms _stfParms;

private:
    static const float g_validZooms[];
};

template <typename PixelT>
FITSWidget::ToQImageVisitor<PixelT>::ToQImageVisitor(ELS::PixSTFParms stfParms)
    : _isColor(false),
      _rIdx(0),
      _gIdx(-1),
      _bIdx(-1),
      _stfParms(stfParms),
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
    double mBal = 0.5;
    double sClip = 0.0;
    double hClip = 1.0;
    double sExp = 0.0;
    double hExp = 1.0;
    if (_isColor)
    {
        PixelT newVals[3];
        int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
        for (int chan = 0; chan < 3; chan++)
        {
            _stfParms.getAll(&mBal, &sClip, &hClip, &sExp, &hExp, chan);
            newVals[chan] = ELS::PixUtils::screenTransferFunc(pixelVals[chanIdx[chan]],
                                                              mBal,
                                                              sClip,
                                                              hClip,
                                                              sExp,
                                                              hExp);
        }

        _qi->setPixelColor(x, y, convertColorPixel(newVals));
    }
    else
    {
        _stfParms.getAll(&mBal, &sClip, &hClip, &sExp, &hExp);
        PixelT val = ELS::PixUtils::screenTransferFunc(pixelVals[0],
                                                       mBal,
                                                       sClip,
                                                       hClip,
                                                       sExp,
                                                       hExp);

        _qi->setPixelColor(x, y, convertGrayPixel(val));
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
    return QColor::fromRgb(val, val, val);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(uint16_t val)
{
    // uint8_t newVal = (uint8_t)(((double)val / ELS::PixUtils::g_u16Max) * ELS::PixUtils::g_u8Max);
    uint8_t newVal = (uint8_t)(val / (ELS::PixUtils::g_u16Max / ELS::PixUtils::g_u8Max));

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(uint32_t val)
{
    uint8_t newVal = (uint8_t)(((double)val / ELS::PixUtils::g_u32Max) * ELS::PixUtils::g_u8Max);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(float val)
{
    uint8_t newVal = (uint8_t)(val * ELS::PixUtils::g_u8Max);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertGrayPixel(double val)
{
    uint8_t newVal = (uint8_t)(val * ELS::PixUtils::g_u8Max);

    return QColor::fromRgb(newVal, newVal, newVal);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const uint8_t *vals)
{
    return QColor::fromRgb(vals[0],
                           vals[1],
                           vals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const uint16_t *vals)
{
    uint8_t newVals[3];
    for (int chan = 0; chan < 3; chan++)
    {
        newVals[chan] = (uint8_t)(((double)vals[chan] / ELS::PixUtils::g_u16Max) * ELS::PixUtils::g_u8Max);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const uint32_t *vals)
{
    uint8_t newVals[3];
    for (int chan = 0; chan < 3; chan++)
    {
        newVals[chan] = (uint8_t)(((double)vals[chan] / ELS::PixUtils::g_u32Max) * ELS::PixUtils::g_u8Max);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const float *vals)
{
    uint8_t newVals[3];
    for (int chan = 0; chan < 3; chan++)
    {
        newVals[chan] = (uint8_t)(vals[chan] * ELS::PixUtils::g_u8Max);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

template <typename PixelT>
QColor FITSWidget::ToQImageVisitor<PixelT>::convertColorPixel(const double *vals)
{
    uint8_t newVals[3];
    for (int chan = 0; chan < 3; chan++)
    {
        newVals[chan] = (uint8_t)(vals[chan] * ELS::PixUtils::g_u8Max);
    }

    return QColor::fromRgb(newVals[0],
                           newVals[1],
                           newVals[2]);
}

#endif // FITSWIDGET_H
