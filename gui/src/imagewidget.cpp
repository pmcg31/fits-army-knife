#include <QPainter>

#include "imageloadexception.h"
#include "imagewidget.h"

/* static */
const float ImageWidget::g_validZooms[] = {
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
    -1.0,
};

ImageWidget::ImageWidget(QWidget* parent)
    : QWidget(parent),
      _sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding),
      _filename(""),
      _image(),
      _zoom(-1.0),
      _actualZoom(-1.0),
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

ImageWidget::~ImageWidget()
{
}

QSize ImageWidget::sizeHint() const
{
    return QSize(800, 600);
}

QSize ImageWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

float ImageWidget::getZoom() const
{
    return _zoom;
}

void ImageWidget::setImage(std::shared_ptr<const QImage> image)
{
    _image = image;

    if (_zoom != -1.0)
    {
        _internalSetZoom(-1.0);
    }

    update();
}

void ImageWidget::setZoom(float zoom)
{
    // Adjust zoom to the closest valid value
    if (zoom != -1.0)
    {
        zoom = adjustZoom(zoom);
    }
    else
    {
        _windowZoomLockPoint = QPoint(width() / 2, height() / 2);
        _imageZoomLockPoint = QPoint(_image->width() / 2, _image->height() / 2);
    }

    if (_zoom != zoom)
    {
        _internalSetZoom(zoom);
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (_mouseDragLast != QPoint(-1, -1))
    {
        QPoint deltas = _mouseDragLast - event->pos();

        QPoint newLockPointZoomed = (_imageZoomLockPoint * _actualZoom) + deltas;

        int imgZoomW = _image->width() * _actualZoom;
        int imgZoomH = _image->height() * _actualZoom;
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

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouseDragLast = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouseDragLast = QPoint(-1, -1);
        setCursor(Qt::OpenHandCursor);
    }
}

void ImageWidget::wheelEvent(QWheelEvent* event)
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

void ImageWidget::paintEvent(QPaintEvent* /* event */)
{
    if (_image != 0)
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

        int imgW = _image->width();
        int imgH = _image->height();

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
        painter.drawImage(_target, *_image, _source);
    }
}

void ImageWidget::_internalSetZoom(float zoom)
{
    _zoom = zoom;

    update();

    emit zoomChanged(_zoom);
}

/* static */
float ImageWidget::adjustZoom(float desiredZoom,
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
