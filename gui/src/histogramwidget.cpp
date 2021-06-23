#include <QPainter>
#include <math.h>

#include "histogramwidget.h"

HistogramWidget::HistogramWidget(QWidget *parent /* = nullptr */)
    : QWidget(parent),
      _isColor(false),
      _numPoints(0),
      _data(0),
      _histImage(0)
{
}

HistogramWidget::~HistogramWidget()
{
    if (_data != 0)
    {
        delete[] _data;
    }
}

QSize HistogramWidget::sizeHint() const
{
    return QSize(500, 200);
}

QSize HistogramWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

void HistogramWidget::setHistogramData(bool isColor,
                                       int numPoints,
                                       uint32_t *data)
{
    _isColor = isColor;
    _numPoints = numPoints;
    _data = data;

    update();
}

void HistogramWidget::paintEvent(QPaintEvent *event)
{
    (void)event;

    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing);

    int realWidth = width();
    int realHeight = height();

    int border = 5;
    int w = realWidth - border;
    int h = realHeight - border;

    double pointsPerPixel = (double)_numPoints / w;

    int histogramSize = w;
    if (_isColor)
    {
        histogramSize *= 3;
    }
    uint32_t *averagedHist = new uint32_t[histogramSize];
    for (int i = 0; i < histogramSize; i++)
    {
        averagedHist[i] = 0;
    }

    double accumulator[3] = {0.0, 0.0, 0.0};
    uint32_t tallest[3] = {0, 0, 0};
    double dataIdxLow = 0.0;
    double dataIdxHigh = dataIdxLow + pointsPerPixel;
    int numChan = 1;
    if (_isColor)
    {
        numChan = 3;
    }
    for (int i = 0; i < w; i++)
    {
        int lowIdxInt = (int)floor(dataIdxLow);
        int highIdxInt = (int)floor(dataIdxHigh);

        for (int chan = 0; chan < numChan; chan++)
        {
            double leftPartial = ((lowIdxInt + 1) - dataIdxLow) * _data[_numPoints * chan + lowIdxInt];
            double rightPartial = (dataIdxHigh - highIdxInt) * _data[_numPoints * chan + highIdxInt];
            accumulator[chan] = leftPartial + rightPartial;
            for (int j = lowIdxInt + 1; j < highIdxInt; j++)
            {
                accumulator[chan] += _data[_numPoints * chan + j];
            }

            averagedHist[w * chan + i] = accumulator[chan] / pointsPerPixel;
            if (averagedHist[w * chan + i] > tallest[chan])
            {
                tallest[chan] = averagedHist[w * chan + i];
            }
        }

        dataIdxLow = dataIdxHigh;
        dataIdxHigh = dataIdxLow + pointsPerPixel;
    }

    if (!_isColor)
    {
        painter.setPen(QColor::fromRgb(160, 160, 160));
        double factor = (double)h / tallest[0];

        for (int x = 0; x < w; x++)
        {
            bool wasZero = averagedHist[x] == 0;
            int val = (int)(averagedHist[x] * factor);
            if ((val == 0) && !wasZero)
            {
                val = 1;
            }
            painter.drawLine(QPoint(border + x, border + h - val),
                             QPoint(border + x, border + h));
        }
    }
    else
    {
        // Determine factor for the tallest peak in all channels
        double factor = 1.0;
        for (int chan = 0; chan < 3; chan++)
        {
            double tmp = (double)h / tallest[chan];
            if (tmp < factor)
            {
                factor = tmp;
            }
        }

        int val[3] = {0, 0, 0};
        for (int x = 0; x < w; x++)
        {
            for (int chan = 0; chan < 3; chan++)
            {
                bool wasZero = averagedHist[w * chan + x] == 0;
                val[chan] = (int)(averagedHist[w * chan + x] * factor);
                if ((val[chan] == 0) && !wasZero)
                {
                    val[chan] = 1;
                }
            }

            int sortedIdx[] = {0, 1, 2};
            int tmp;
            if (val[sortedIdx[0]] < val[sortedIdx[1]])
            {
                if (val[sortedIdx[1]] > val[sortedIdx[2]])
                {
                    tmp = sortedIdx[1];
                    sortedIdx[1] = sortedIdx[2];
                    sortedIdx[2] = tmp;

                    if (val[sortedIdx[0]] > val[sortedIdx[1]])
                    {
                        tmp = sortedIdx[0];
                        sortedIdx[0] = sortedIdx[1];
                        sortedIdx[1] = tmp;
                    }
                }
            }
            else
            {
                tmp = sortedIdx[0];
                sortedIdx[0] = sortedIdx[1];
                sortedIdx[1] = tmp;

                if (val[sortedIdx[1]] > val[sortedIdx[2]])
                {
                    tmp = sortedIdx[1];
                    sortedIdx[1] = sortedIdx[2];
                    sortedIdx[2] = tmp;
                }

                if (val[sortedIdx[0]] > val[sortedIdx[1]])
                {
                    tmp = sortedIdx[0];
                    sortedIdx[0] = sortedIdx[1];
                    sortedIdx[1] = tmp;
                }
            }

            // Gray; the portion all channels occupy
            int rgb[] = {160, 160, 160};
            painter.setPen(QColor::fromRgb(rgb[0], rgb[1], rgb[2]));
            painter.drawLine(QPoint(border + x, border + h - val[sortedIdx[0]]),
                             QPoint(border + x, border + h));
            rgb[sortedIdx[0]] = 0;

            // Bi-color, the middle and highest channel
            painter.setPen(QColor::fromRgb(rgb[0], rgb[1], rgb[2]));
            painter.drawLine(QPoint(border + x, border + h - val[sortedIdx[1]]),
                             QPoint(border + x, border + h - val[sortedIdx[0]]));
            rgb[sortedIdx[1]] = 0;

            // Single color; the highest channel
            painter.setPen(QColor::fromRgb(rgb[0], rgb[1], rgb[2]));
            painter.drawLine(QPoint(border + x, border + h - val[sortedIdx[2]]),
                             QPoint(border + x, border + h - val[sortedIdx[1]]));
        }
    }
}
