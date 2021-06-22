#include <QPainter>

#include "fitswidget.h"
#include "fitstantrum.h"

#define FITS_READ_T double

FITSWidget::FITSWidget(QWidget *parent)
    : QWidget(parent), _sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding), _filename(0), _fits(0), _cacheImage(0)
{
    setBackgroundRole(QPalette::Dark);
    setAutoFillBackground(true);

    setSizePolicy(_sizePolicy);
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

void FITSWidget::setFile(const char *filename)
{
    if ((_filename == 0) || (strcmp(filename, _filename) != 0))
    {
        try
        {
            ELS::FITSImage *tmpFits = ELS::FITSImage::load(filename);

            if (_fits != 0)
            {
                delete _fits;
            }

            _filename = filename;
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

void FITSWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    int w = width();
    int h = height();

    painter.drawRect(0, 0, w, h);

    if (_cacheImage == 0)
    {
        _cacheImage = convertImage();
    }

    int imgW = _fits->getWidth();
    int imgH = _fits->getHeight();

    QRect target;
    if ((imgW < w) && (imgH < h))
    {
        target.setLeft((w - imgW) / 2);
        target.setTop((h - imgH) / 2);
        target.setWidth(imgW);
        target.setHeight(imgH);
    }
    else
    {
        float imgAspect = (float)imgW / (float)imgH;
        float winAspect = (float)w / (float)h;

        if (imgAspect >= winAspect)
        {
            target.setLeft(0);
            target.setWidth(w);
            int targetHeight = (int)(w / imgAspect);
            target.setTop((h - targetHeight) / 2);
            target.setHeight(targetHeight);
        }
        else
        {
            target.setTop(0);
            target.setHeight(h);
            int targetWidth = (int)(h * imgAspect);
            target.setLeft((w - targetWidth) / 2);
            target.setWidth(targetWidth);
        }
    }

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(target, *_cacheImage);
}

QImage *FITSWidget::convertImage() const
{
    QImage::Format format = QImage::Format_ARGB32;

    int width = _fits->getWidth();
    int height = _fits->getHeight();
    ELS::FITSImage::PixelFormat pf = _fits->getPixelFormat();
    bool isColor = _fits->isColor();

    QImage *qi = new QImage(width,
                            height,
                            format);

    const void *pixels = _fits->getPixels();

    switch (_fits->getBitDepth())
    {
    case ELS::FITSImage::BD_INT_8:
        break;
    case ELS::FITSImage::BD_INT_16:
        if (isColor)
        {
            convertU16ColorImage(qi,
                                 width,
                                 height,
                                 pf,
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
    case ELS::FITSImage::BD_INT_32:
        break;
    case ELS::FITSImage::BD_FLOAT:
        if (isColor)
        {
            convertFloatColorImage(qi,
                                   width,
                                   height,
                                   pf,
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
    case ELS::FITSImage::BD_DOUBLE:
        if (isColor)
        {
            convertDoubleColorImage(qi,
                                    width,
                                    height,
                                    pf,
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
                                      ELS::FITSImage::PixelFormat pf,
                                      const uint16_t *pixels) const
{
    uint8_t valR = 0;
    uint8_t valG = 0;
    uint8_t valB = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            switch (pf)
            {
            case ELS::FITSImage::PF_INTERLEAVED:
                valR = (uint8_t)((double)pixels[3 * (x + width * y) + 2] / 257);
                valG = (uint8_t)((double)pixels[3 * (x + width * y) + 1] / 257);
                valB = (uint8_t)((double)pixels[3 * (x + width * y) + 0] / 257);
                break;
            case ELS::FITSImage::PF_STRIDED:
                valR = (uint8_t)((double)pixels[x + width * y + 2 * width * height] / 257);
                valG = (uint8_t)((double)pixels[x + width * y + 1 * width * height] / 257);
                valB = (uint8_t)((double)pixels[x + width * y + 0 * width * height] / 257);
                break;
            }

            qi->setPixelColor(x, y, QColor::fromRgb(valR, valG, valB));
        }
    }
}

void FITSWidget::convertFloatColorImage(QImage *qi,
                                        int width,
                                        int height,
                                        ELS::FITSImage::PixelFormat pf,
                                        const float *pixels) const
{
    uint8_t valR = 0;
    uint8_t valG = 0;
    uint8_t valB = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            switch (pf)
            {
            case ELS::FITSImage::PF_INTERLEAVED:
                valR = 255 * pixels[3 * (x + width * y) + 2];
                valG = 255 * pixels[3 * (x + width * y) + 1];
                valB = 255 * pixels[3 * (x + width * y) + 0];
                break;
            case ELS::FITSImage::PF_STRIDED:
                valR = 255 * pixels[x + width * y + 2 * width * height];
                valG = 255 * pixels[x + width * y + 1 * width * height];
                valB = 255 * pixels[x + width * y + 0 * width * height];
                break;
            }

            qi->setPixelColor(x, y, QColor::fromRgb(valR, valG, valB));
        }
    }
}

void FITSWidget::convertDoubleColorImage(QImage *qi,
                                         int width,
                                         int height,
                                         ELS::FITSImage::PixelFormat pf,
                                         const double *pixels) const
{
    uint8_t valR = 0;
    uint8_t valG = 0;
    uint8_t valB = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            switch (pf)
            {
            case ELS::FITSImage::PF_INTERLEAVED:
                valR = 255 * pixels[3 * (x + width * y) + 2];
                valG = 255 * pixels[3 * (x + width * y) + 1];
                valB = 255 * pixels[3 * (x + width * y) + 0];
                break;
            case ELS::FITSImage::PF_STRIDED:
                valR = 255 * pixels[x + width * y + 2 * width * height];
                valG = 255 * pixels[x + width * y + 1 * width * height];
                valB = 255 * pixels[x + width * y + 0 * width * height];
                break;
            }

            qi->setPixelColor(x, y, QColor::fromRgb(valR, valG, valB));
        }
    }
}
