#include <QPainter>

#include "fitswidget.h"
#include "fitstantrum.h"

/* static */
const float FITSWidget::g_validZooms[] = {
    0.125,
    0.250,
    0.333,
    0.500,
    0.667,
    0.750,
    1.000,
    1.250,
    1.333,
    1.500,
    1.667,
    1.750,
    2.000,
    -1.0};

FITSWidget::FITSWidget(QWidget *parent)
    : QWidget(parent),
      _sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding),
      _filename(""),
      _fits(0),
      _cacheImage(0),
      _zoom(-1.0),
      _actualZoom(-1.0)
{
    setBackgroundRole(QPalette::Dark);
    setAutoFillBackground(true);

    setSizePolicy(_sizePolicy);
}

FITSWidget::~FITSWidget()
{
    if (_fits != 0)
    {
        delete _fits;
    }

    if (_cacheImage != 0)
    {
        delete _cacheImage;
    }
}

QSize FITSWidget::sizeHint() const
{
    return QSize(800, 600);
}

QSize FITSWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

const ELS::FITSImage *FITSWidget::getImage() const
{
    return _fits;
}

const char *FITSWidget::getFilename() const
{
    return _filename;
}

float FITSWidget::getZoom() const
{
    return _zoom;
}

void FITSWidget::setFile(const char *filename)
{
    if (strcmp(filename, _filename) != 0)
    {
        try
        {
            ELS::FITSImage *tmpFits = ELS::FITSImage::load(filename);

            if (_fits != 0)
            {
                delete _fits;
            }

            strcpy(_filename, filename);
            _fits = tmpFits;

            emit fileChanged(_filename);
        }
        catch (ELS::FITSException *e)
        {
            fprintf(stderr, "FITSException: %s for file %s\n", e->getErrText(), filename);
            delete e;
        }
    }
}

void FITSWidget::setZoom(float zoom)
{
    // Adjust zoom to the closest valid value
    if (zoom != -1.0)
    {
        zoom = adjustZoom(zoom);
    }

    if (_zoom != zoom)
    {
        _internalSetZoom(zoom);
    }
}

void FITSWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numSteps = event->angleDelta() / 120;

    float zoom = _actualZoom;
    if (numSteps.y() >= 1)
    {
        zoom = adjustZoom(zoom, ZAS_HIGHER);

        _internalSetZoom(zoom);
    }
    else if (numSteps.y() <= -1)
    {
        zoom = adjustZoom(zoom, ZAS_LOWER);

        _internalSetZoom(zoom);
    }
}

void FITSWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    int realWidth = width();
    int realHeight = height();

    // painter.drawRect(0, 0, realWidth, realHeight);

    int border = 1;

    int w = realWidth - (border * 2);
    int h = realHeight - (border * 2);

    if (_cacheImage == 0)
    {
        _cacheImage = convertImage();
    }

    int imgW = _fits->getWidth();
    int imgH = _fits->getHeight();
    int imgZoomW = imgW;
    int imgZoomH = imgH;

    if (_zoom != -1.0)
    {
        imgZoomW *= _zoom;
        imgZoomH *= _zoom;
    }

    QRect target;
    QRect source(0, 0, imgW, imgH);
    float zoomNow = _zoom;
    if ((imgZoomW < w) && (imgZoomH < h))
    {
        target.setLeft((w - imgZoomW) / 2);
        target.setTop((h - imgZoomH) / 2);
        target.setWidth(imgZoomW);
        target.setHeight(imgZoomH);
    }
    else
    {
        if (_zoom == -1.0)
        {
            float imgAspect = (float)imgW / (float)imgH;
            float winAspect = (float)w / (float)h;

            if (imgAspect >= winAspect)
            {
                target.setLeft(border);
                target.setWidth(w);
                int targetHeight = (int)(w / imgAspect);
                target.setTop((h - targetHeight) / 2 + border);
                target.setHeight(targetHeight);
            }
            else
            {
                target.setTop(border);
                target.setHeight(h);
                int targetWidth = (int)(h * imgAspect);
                target.setLeft((w - targetWidth) / 2 + border);
                target.setWidth(targetWidth);
            }

            zoomNow = (float)target.width() / (float)imgW;
        }
        else
        {
            int widthXtra = imgZoomW - w;
            int heightXtra = imgZoomH - h;

            QRect sourceZoom(0, 0, 0, 0);
            if (widthXtra < 0)
            {
                target.setLeft((w - imgZoomW) / 2 + border);
                target.setWidth(imgZoomW);
                sourceZoom.setLeft(0);
                sourceZoom.setWidth(imgZoomW);
            }
            else
            {
                target.setLeft(border);
                target.setWidth(w);
                sourceZoom.setLeft(widthXtra / 2);
                sourceZoom.setWidth(w);
            }

            if (heightXtra < 0)
            {
                target.setTop((h - imgZoomH) / 2 + border);
                target.setHeight(imgZoomH);
                sourceZoom.setTop(0);
                sourceZoom.setHeight(imgZoomH);
            }
            else
            {
                target.setTop(border);
                target.setHeight(h);
                sourceZoom.setTop(heightXtra / 2);
                sourceZoom.setHeight(h);
            }

            source.setLeft(sourceZoom.left() / _zoom);
            source.setWidth(sourceZoom.width() / _zoom);
            source.setTop(sourceZoom.top() / _zoom);
            source.setHeight(sourceZoom.height() / _zoom);
        }
    }

    if (zoomNow != _actualZoom)
    {
        _actualZoom = zoomNow;

        emit actualZoomChanged(_actualZoom);
    }

    painter.setRenderHints(QPainter::SmoothPixmapTransform |
                           QPainter::Antialiasing);
    painter.drawImage(target, *_cacheImage, source);
}

QImage *FITSWidget::convertImage() const
{
    QImage::Format format = QImage::Format_ARGB32;

    int width = _fits->getWidth();
    int height = _fits->getHeight();
    ELS::FITS::RasterFormat rf = _fits->getRasterFormat();
    bool isColor = _fits->isColor();

    QImage *qi = new QImage(width,
                            height,
                            format);

    const void *pixels = _fits->getPixels();

    switch (_fits->getBitDepth())
    {
    case ELS::FITS::BD_INT_8:
        break;
    case ELS::FITS::BD_INT_16:
        if (isColor)
        {
            convertU16ColorImage(qi,
                                 width,
                                 height,
                                 rf,
                                 (const uint16_t *)pixels);
        }
        else
        {
            convertU16MonoImage(qi,
                                width,
                                height,
                                (const uint16_t *)pixels);
        }
        break;
    case ELS::FITS::BD_INT_32:
        break;
    case ELS::FITS::BD_FLOAT:
        if (isColor)
        {
            convertFloatColorImage(qi,
                                   width,
                                   height,
                                   rf,
                                   (const float *)pixels);
        }
        else
        {
            convertFloatMonoImage(qi,
                                  width,
                                  height,
                                  (const float *)pixels);
        }
        break;
    case ELS::FITS::BD_DOUBLE:
        if (isColor)
        {
            convertDoubleColorImage(qi,
                                    width,
                                    height,
                                    rf,
                                    (const double *)pixels);
        }
        else
        {
            convertDoubleMonoImage(qi,
                                   width,
                                   height,
                                   (const double *)pixels);
        }
        break;
    }

    return qi;
}

void FITSWidget::convertU16MonoImage(QImage *qi,
                                     int width,
                                     int height,
                                     const uint16_t *pixels) const
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t val = (uint8_t)((double)pixels[y * width + x] / 257);
            qi->setPixelColor(x, y, QColor::fromRgb(val, val, val));
        }
    }
}

void FITSWidget::convertFloatMonoImage(QImage *qi,
                                       int width,
                                       int height,
                                       const float *pixels) const
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t val = (uint8_t)(pixels[y * width + x] * 255);
            qi->setPixelColor(x, y, QColor::fromRgb(val, val, val));
        }
    }
}

void FITSWidget::convertDoubleMonoImage(QImage *qi,
                                        int width,
                                        int height,
                                        const double *pixels) const
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t val = (uint8_t)(pixels[y * width + x] * 255);
            qi->setPixelColor(x, y, QColor::fromRgb(val, val, val));
        }
    }
}

void FITSWidget::convertU16ColorImage(QImage *qi,
                                      int width,
                                      int height,
                                      ELS::FITS::RasterFormat rf,
                                      const uint16_t *pixels) const
{
    uint8_t valR = 0;
    uint8_t valG = 0;
    uint8_t valB = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            switch (rf)
            {
            case ELS::FITS::RF_INTERLEAVED:
                valR = (uint8_t)((double)pixels[3 * (x + width * y) + 0] / 257);
                valG = (uint8_t)((double)pixels[3 * (x + width * y) + 1] / 257);
                valB = (uint8_t)((double)pixels[3 * (x + width * y) + 2] / 257);
                break;
            case ELS::FITS::RF_STRIDED:
                valR = (uint8_t)((double)pixels[x + width * y + 0 * width * height] / 257);
                valG = (uint8_t)((double)pixels[x + width * y + 1 * width * height] / 257);
                valB = (uint8_t)((double)pixels[x + width * y + 2 * width * height] / 257);
                break;
            }

            qi->setPixelColor(x, y, QColor::fromRgb(valR, valG, valB));
        }
    }
}

void FITSWidget::convertFloatColorImage(QImage *qi,
                                        int width,
                                        int height,
                                        ELS::FITS::RasterFormat rf,
                                        const float *pixels) const
{
    uint8_t valR = 0;
    uint8_t valG = 0;
    uint8_t valB = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            switch (rf)
            {
            case ELS::FITS::RF_INTERLEAVED:
                valR = 255 * pixels[3 * (x + width * y) + 0];
                valG = 255 * pixels[3 * (x + width * y) + 1];
                valB = 255 * pixels[3 * (x + width * y) + 2];
                break;
            case ELS::FITS::RF_STRIDED:
                valR = 255 * pixels[x + width * y + 0 * width * height];
                valG = 255 * pixels[x + width * y + 1 * width * height];
                valB = 255 * pixels[x + width * y + 2 * width * height];
                break;
            }

            qi->setPixelColor(x, y, QColor::fromRgb(valR, valG, valB));
        }
    }
}

void FITSWidget::convertDoubleColorImage(QImage *qi,
                                         int width,
                                         int height,
                                         ELS::FITS::RasterFormat rf,
                                         const double *pixels) const
{
    uint8_t valR = 0;
    uint8_t valG = 0;
    uint8_t valB = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            switch (rf)
            {
            case ELS::FITS::RF_INTERLEAVED:
                valR = 255 * pixels[3 * (x + width * y) + 0];
                valG = 255 * pixels[3 * (x + width * y) + 1];
                valB = 255 * pixels[3 * (x + width * y) + 2];
                break;
            case ELS::FITS::RF_STRIDED:
                valR = 255 * pixels[x + width * y + 0 * width * height];
                valG = 255 * pixels[x + width * y + 1 * width * height];
                valB = 255 * pixels[x + width * y + 2 * width * height];
                break;
            }

            qi->setPixelColor(x, y, QColor::fromRgb(valR, valG, valB));
        }
    }
}

void FITSWidget::_internalSetZoom(float zoom)
{
    _zoom = zoom;

    update();

    emit zoomChanged(_zoom);
}

/* static */
float FITSWidget::adjustZoom(float desiredZoom,
                             ZoomAdjustStrategy strategy /* = ZAS_CLOSEST */)
{
    if (desiredZoom < g_validZooms[0])
    {
        return g_validZooms[0];
    }
    else
    {
        if (desiredZoom == g_validZooms[0])
        {
            if (strategy == ZAS_HIGHER)
            {
                return g_validZooms[1];
            }
            else
            {
                return g_validZooms[0];
            }
        }
        else
        {
            for (int i = 1; true; i++)
            {
                if (g_validZooms[i] == -1.0)
                {
                    return g_validZooms[i - 1];
                }

                if (desiredZoom == g_validZooms[i])
                {
                    switch (strategy)
                    {
                    case ZAS_CLOSEST:
                        return g_validZooms[i];
                    case ZAS_HIGHER:
                        break;
                    case ZAS_LOWER:
                        return g_validZooms[i - 1];
                    }

                    continue;
                }

                if (desiredZoom < g_validZooms[i])
                {
                    switch (strategy)
                    {
                    case ZAS_CLOSEST:
                    {
                        float tmp1 = g_validZooms[i] - desiredZoom;
                        float tmp2 = desiredZoom - g_validZooms[i - 1];

                        if (tmp1 < tmp2)
                        {
                            return g_validZooms[i];
                        }
                        else
                        {
                            return g_validZooms[i - 1];
                        }
                    }
                    case ZAS_HIGHER:
                        return g_validZooms[i];
                    case ZAS_LOWER:
                        return g_validZooms[i - 1];
                    }

                    break;
                }
            }
        }
    }

    return g_validZooms[0];
}
