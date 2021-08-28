#include <QPainter>

#include "fitstantrum.h"
#include "fitswidget.h"

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

FITSWidget::FITSWidget(QWidget* parent)
    : QWidget(parent),
      _sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding),
      _filename(""),
      _fits(0),
      _cacheImage(),
      _cacheImageData(),
      _showStretched(false),
      _zoom(-1.0),
      _actualZoom(-1.0),
      _stfParms(),
      _isColor(false),
      _numHistogramPoints(0),
      _histogramData(),
      _stfLUT(0),
      _identityLUT(0),
      _lutInUse(_identityLUT),
      _mouseDragLast(-1, -1),
      _source(0, 0, 0, 0),
      _target(0, 0, 0, 0),
      _imageZoomLockPoint(-1, -1),
      _windowZoomLockPoint(-1, -1)
{
    setBackgroundRole(QPalette::Dark);
    setAutoFillBackground(true);

    setSizePolicy(_sizePolicy);

    setCursor(Qt::OpenHandCursor);
}

FITSWidget::~FITSWidget()
{
    if (_fits != 0)
    {
        delete _fits;
        _fits = 0;
    }

    if (_identityLUT != 0)
    {
        delete[] _identityLUT;
        _identityLUT = 0;
    }

    if (_stfLUT != 0)
    {
        delete[] _stfLUT;
        _stfLUT = 0;
    }

    _lutInUse = 0;
}

QSize FITSWidget::sizeHint() const
{
    return QSize(800, 600);
}

QSize FITSWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

const ELS::FITSImage* FITSWidget::getImage() const
{
    return _fits;
}

const char* FITSWidget::getFilename() const
{
    return _filename;
}

bool FITSWidget::getStretched() const
{
    return _showStretched;
}

float FITSWidget::getZoom() const
{
    return _zoom;
}

void FITSWidget::setFile(const char* filename)
{
    if (strcmp(filename, _filename) != 0)
    {
        try
        {
            ELS::FITSImage* tmpFits = ELS::FITSImage::load(filename);

            if (_fits != 0)
            {
                delete _fits;
            }

            if (_cacheImage != 0)
            {
                _cacheImage.reset();
                _cacheImageData.reset();
            }

            strcpy(_filename, filename);
            _fits = tmpFits;

            emit fileChanged(_filename);
        }
        catch (ELS::FITSException* e)
        {
            fprintf(stderr, "FITSException: %s for file %s\n", e->getErrText(), filename);
            delete e;
        }
    }
}

void FITSWidget::showStretched(ELS::PixSTFParms stfParms)
{
    if (_showStretched != true)
    {
        _showStretched = true;

        if ((_stfLUT == 0) || (_stfParms != stfParms))
        {
            _stfParms = stfParms;
            calculateStfLUT();
        }
        _lutInUse = _stfLUT;

        if (_cacheImage != 0)
        {
            _cacheImage.reset();
            _cacheImageData.reset();
        }

        update();
    }
}

void FITSWidget::clearStretched()
{
    if (_showStretched != false)
    {
        _showStretched = false;

        _stfParms = ELS::PixSTFParms();
        _lutInUse = _identityLUT;

        if (_cacheImage != 0)
        {
            _cacheImage.reset();
            _cacheImageData.reset();
        }

        update();
    }
}

void FITSWidget::setZoom(float zoom)
{
    // Adjust zoom to the closest valid value
    if (zoom != -1.0)
    {
        zoom = adjustZoom(zoom);
    }
    else
    {
        _windowZoomLockPoint = QPoint(width() / 2, height() / 2);
        _imageZoomLockPoint = QPoint(_fits->getWidth() / 2, _fits->getHeight() / 2);
    }

    if (_zoom != zoom)
    {
        _internalSetZoom(zoom);
    }
}

void FITSWidget::setHistogramData(bool isColor,
                                  int numPoints,
                                  std::shared_ptr<uint32_t[]> data)
{
    _isColor = isColor;
    _numHistogramPoints = numPoints;
    _histogramData = data;

    int totalHistogramPoints = _numHistogramPoints;
    if (_isColor)
    {
        totalHistogramPoints *= 3;
    }
    _identityLUT = new uint8_t[totalHistogramPoints];
    _lutInUse = _identityLUT;

    ELS::PixSTFParms stfParms;
    double mBal = 0.5;
    double sClip = 0.0;
    double hClip = 1.0;
    double sExp = 0.0;
    double hExp = 1.0;
    for (uint16_t i = 0; i < _numHistogramPoints; i++)
    {
        if (_isColor)
        {
            int chanOffset[3] = {
                0,
                _numHistogramPoints,
                _numHistogramPoints * 2};
            for (int chan = 0; chan < 3; chan++)
            {
                stfParms.getAll(&mBal, &sClip, &hClip, &sExp, &hExp, chan);
                _identityLUT[chanOffset[chan] + i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                                                 mBal,
                                                                                                 sClip,
                                                                                                 hClip,
                                                                                                 sExp,
                                                                                                 hExp) *
                                                               ELS::PixUtils::g_u8Max);
            }
        }
        else
        {
            stfParms.getAll(&mBal, &sClip, &hClip, &sExp, &hExp);
            _identityLUT[i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                          mBal,
                                                                          sClip,
                                                                          hClip,
                                                                          sExp,
                                                                          hExp) *
                                        ELS::PixUtils::g_u8Max);
        }
    }

    update();
}

void FITSWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (_mouseDragLast != QPoint(-1, -1))
    {
        QPoint deltas = _mouseDragLast - event->pos();

        QPoint newLockPointZoomed = (_imageZoomLockPoint * _actualZoom) + deltas;

        int imgZoomW = _fits->getWidth() * _actualZoom;
        int imgZoomH = _fits->getHeight() * _actualZoom;
        newLockPointZoomed.setX(std::max(_windowZoomLockPoint.x(),
                                         std::min(imgZoomW - (_target.width() - _windowZoomLockPoint.x()),
                                                  newLockPointZoomed.x())));
        newLockPointZoomed.setY(std::max(_windowZoomLockPoint.y(),
                                         std::min(imgZoomH - (_target.height() - _windowZoomLockPoint.y()),
                                                  newLockPointZoomed.y())));

        _imageZoomLockPoint = newLockPointZoomed / _actualZoom;

        _mouseDragLast = event->pos();
        update();
    }
}

void FITSWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouseDragLast = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void FITSWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouseDragLast = QPoint(-1, -1);
        setCursor(Qt::OpenHandCursor);
    }
}

void FITSWidget::wheelEvent(QWheelEvent* event)
{
    QPoint numSteps = event->angleDelta() / 120;

    if (!_source.isNull())
    {
        _windowZoomLockPoint = event->position().toPoint();
        QPoint sourceTopLeftZoomed = _source.topLeft() * _actualZoom;
        _imageZoomLockPoint = (sourceTopLeftZoomed + (_windowZoomLockPoint - _target.topLeft())) / _actualZoom;
    }

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

void FITSWidget::paintEvent(QPaintEvent* /* event */)
{
    if (_lutInUse != 0)
    {
        QPainter painter(this);

        int realWidth = width();
        int realHeight = height();

        if (_windowZoomLockPoint == QPoint(-1, -1))
        {
            _windowZoomLockPoint = QPoint(realWidth / 2, realHeight / 2);
        }

        int border = 1;

        int w = realWidth - (border * 2);
        int h = realHeight - (border * 2);

        if (_cacheImage == 0)
        {
            ToQImageVisitor visitor(_stfParms, _lutInUse, _numHistogramPoints);
            _fits->visitPixels(&visitor);
            _cacheImageData = visitor.getImageData();
            _cacheImage = visitor.getImage();
            // convertImage();
        }

        int imgW = _fits->getWidth();
        int imgH = _fits->getHeight();

        if (_imageZoomLockPoint == QPoint(-1, -1))
        {
            _imageZoomLockPoint = QPoint(imgW / 2, imgH / 2);
        }

        int imgZoomW = imgW;
        int imgZoomH = imgH;
        if (_zoom != -1.0)
        {
            imgZoomW *= _zoom;
            imgZoomH *= _zoom;
        }
        QPoint imgCenterZoom(imgZoomW / 2, imgZoomH / 2);

        _source = QRect(0, 0, imgW, imgH);
        float zoomNow = _zoom;
        if ((imgZoomW < w) && (imgZoomH < h))
        {
            _target.setLeft((w - imgZoomW) / 2);
            _target.setTop((h - imgZoomH) / 2);
            _target.setWidth(imgZoomW);
            _target.setHeight(imgZoomH);
        }
        else
        {
            if (_zoom == -1.0)
            {
                float imgAspect = (float)imgW / (float)imgH;
                float winAspect = (float)w / (float)h;

                if (imgAspect >= winAspect)
                {
                    _target.setLeft(border);
                    _target.setWidth(w);
                    int targetHeight = (int)(w / imgAspect);
                    _target.setTop((h - targetHeight) / 2 + border);
                    _target.setHeight(targetHeight);
                }
                else
                {
                    _target.setTop(border);
                    _target.setHeight(h);
                    int targetWidth = (int)(h * imgAspect);
                    _target.setLeft((w - targetWidth) / 2 + border);
                    _target.setWidth(targetWidth);
                }

                zoomNow = (float)_target.width() / (float)imgW;
            }
            else
            {
                int widthXtra = imgZoomW - w;
                int heightXtra = imgZoomH - h;

                QPoint imgZoomLockPointZoomed = _imageZoomLockPoint * _zoom;
                QPoint sourceZoomedTopLeft = imgZoomLockPointZoomed - _windowZoomLockPoint;

                QRect sourceZoom(0, 0, 0, 0);
                if (widthXtra < 0)
                {
                    _target.setLeft((w - imgZoomW) / 2 + border);
                    _target.setWidth(imgZoomW);
                    sourceZoom.setLeft(0);
                    sourceZoom.setWidth(imgZoomW);
                }
                else
                {
                    _target.setLeft(border);
                    _target.setWidth(w);
                    int left = std::max(0, std::min(sourceZoomedTopLeft.x(), widthXtra));
                    sourceZoom.setLeft(left);
                    sourceZoom.setWidth(w);
                }

                if (heightXtra < 0)
                {
                    _target.setTop((h - imgZoomH) / 2 + border);
                    _target.setHeight(imgZoomH);
                    sourceZoom.setTop(0);
                    sourceZoom.setHeight(imgZoomH);
                }
                else
                {
                    _target.setTop(border);
                    _target.setHeight(h);
                    int top = std::max(0, std::min(sourceZoomedTopLeft.y(), heightXtra));
                    sourceZoom.setTop(top);
                    sourceZoom.setHeight(h);
                }

                _source.setLeft(sourceZoom.left() / _zoom);
                _source.setWidth(sourceZoom.width() / _zoom);
                _source.setTop(sourceZoom.top() / _zoom);
                _source.setHeight(sourceZoom.height() / _zoom);
            }
        }

        if (zoomNow != _actualZoom)
        {
            _actualZoom = zoomNow;

            emit actualZoomChanged(_actualZoom);
        }

        painter.setRenderHints(QPainter::SmoothPixmapTransform |
                               QPainter::Antialiasing);
        painter.drawImage(_target, *_cacheImage, _source);
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

void FITSWidget::calculateStfLUT()
{
    if (_stfLUT != 0)
    {
        delete[] _stfLUT;
    }

    int totalHistogramPoints = _numHistogramPoints;
    if (_isColor)
    {
        totalHistogramPoints *= 3;
    }
    _stfLUT = new uint8_t[totalHistogramPoints];

    double mBal = 0.5;
    double sClip = 0.0;
    double hClip = 1.0;
    double sExp = 0.0;
    double hExp = 1.0;
    for (uint16_t i = 0; i < _numHistogramPoints; i++)
    {
        // Only calculate points that are in the image
        if (_histogramData[i] != 0)
        {
            if (_isColor)
            {
                int chanOffset[3] = {
                    0,
                    _numHistogramPoints,
                    _numHistogramPoints * 2};
                for (int chan = 0; chan < 3; chan++)
                {
                    _stfParms.getAll(&mBal, &sClip, &hClip, &sExp, &hExp, chan);
                    _stfLUT[chanOffset[chan] + i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                                                mBal,
                                                                                                sClip,
                                                                                                hClip,
                                                                                                sExp,
                                                                                                hExp) *
                                                              ELS::PixUtils::g_u8Max);
                }
            }
            else
            {
                _stfParms.getAll(&mBal, &sClip, &hClip, &sExp, &hExp);
                _stfLUT[i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                         mBal,
                                                                         sClip,
                                                                         hClip,
                                                                         sExp,
                                                                         hExp) *
                                       ELS::PixUtils::g_u8Max);
            }
        }
    }
}

FITSWidget::ToQImageVisitor::ToQImageVisitor(ELS::PixSTFParms stfParms,
                                             uint8_t* lut,
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

FITSWidget::ToQImageVisitor::~ToQImageVisitor()
{
}

std::shared_ptr<uint32_t[]> FITSWidget::ToQImageVisitor::getImageData()
{
    return _qiData;
}

std::shared_ptr<QImage> FITSWidget::ToQImageVisitor::getImage()
{
    return _qi;
}

void FITSWidget::ToQImageVisitor::pixelFormat(ELS::PixelFormat pf)
{
    (void)pf;
}

void FITSWidget::ToQImageVisitor::dimensions(int width,
                                             int height)
{
    _width = width;
    _height = height;
    _pixCount = _width * _height;

    _qiData.reset(new uint32_t[_pixCount]);
}

void FITSWidget::ToQImageVisitor::rowInfo(int stride)
{
    _stride = stride;
}

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const int8_t* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const int16_t* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const int32_t* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const uint8_t* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const uint16_t* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const uint32_t* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const float* k)
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

void FITSWidget::ToQImageVisitor::rowGray(int y,
                                          const double* k)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const int8_t* r,
                                         const int8_t* g,
                                         const int8_t* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const int16_t* r,
                                         const int16_t* g,
                                         const int16_t* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const int32_t* r,
                                         const int32_t* g,
                                         const int32_t* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const uint8_t* r,
                                         const uint8_t* g,
                                         const uint8_t* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const uint16_t* r,
                                         const uint16_t* g,
                                         const uint16_t* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const uint32_t* r,
                                         const uint32_t* g,
                                         const uint32_t* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const float* r,
                                         const float* g,
                                         const float* b)
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

void FITSWidget::ToQImageVisitor::rowRgb(int y,
                                         const double* r,
                                         const double* g,
                                         const double* b)
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

void FITSWidget::ToQImageVisitor::done()
{
    _qi.reset(new QImage((const uchar*)_qiData.get(), _width, _height, QImage::Format_ARGB32));
}
