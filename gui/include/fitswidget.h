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
        virtual void pixel(int x, int y, const PixelT *pixelVals) override;
        virtual void done() override;

    private:
        bool _isColor;
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
    ELS::PixSTFParms _stfParms;
    bool _isColor;
    int _numHistogramPoints;
    std::shared_ptr<uint32_t[]> _histogramData;
    uint8_t *_stfLUT;
    uint8_t *_identityLUT;
    uint8_t *_lutInUse;

private:
    static const float g_validZooms[];
};

template <typename PixelT>
FITSWidget::ToQImageVisitor<PixelT>::ToQImageVisitor(ELS::PixSTFParms stfParms,
                                                     uint8_t *lut,
                                                     int lutPoints)
    : _isColor(false),
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
    _qi.reset(new QImage(width, height, QImage::Format_ARGB32));
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::pixel(int x,
                                                int y,
                                                const PixelT *pixelVals)
{
    if (_isColor)
    {
        uint8_t newVals[3];
        int chanIdx[3] = {_rIdx, _gIdx, _bIdx};
        for (int chan = 0; chan < 3; chan++)
        {
            uint16_t tmp = ELS::PixUtils::convertRangeToHist(pixelVals[chanIdx[chan]]);
            newVals[chan] = _lut[tmp];
        }

        _qi->setPixelColor(x, y, QColor::fromRgb(newVals[0], newVals[1], newVals[2]));
    }
    else
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(pixelVals[0]);
        uint8_t val = _lut[tmp];

        _qi->setPixelColor(x, y, QColor::fromRgb(val, val, val));
    }
}

template <typename PixelT>
void FITSWidget::ToQImageVisitor<PixelT>::done()
{
    // Cool
}

#endif // FITSWIDGET_H
