#pragma once

#include <memory>
#include <inttypes.h>
#include "pixutils.h"
#include "pixstatistics.h"
#include "pixelvisitor.h"

namespace ELS
{

    template <typename PixelT>
    class StatisticsVisitor : public ELS::PixelVisitor<PixelT>
    {
    public:
        StatisticsVisitor();
        ~StatisticsVisitor();

        PixStatistics<PixelT> getStatistics() const;

        void getHistogramData(int *numPoints,
                              std::shared_ptr<uint32_t[]> *data);

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
        bool _isColor;
        int _width;
        int _stride;
        bool _isFirstPixel;
        int64_t _pixelCount;
        std::shared_ptr<uint32_t[]> _histogram;
        double _accumulator[3];
        PixelT _minVal[3];
        PixelT _maxVal[3];
        PixStatistics<PixelT> _statistics;
    };

    template <typename PixelT>
    StatisticsVisitor<PixelT>::StatisticsVisitor()
        : _isColor(false),
          _width(0),
          _stride(0),
          _isFirstPixel(true),
          _histogram(0),
          _accumulator{0.0, 0.0, 0.0},
          _minVal{0, 0, 0},
          _maxVal{0, 0, 0},
          _statistics()
    {
    }

    template <typename PixelT>
    StatisticsVisitor<PixelT>::~StatisticsVisitor()
    {
    }

    template <typename PixelT>
    PixStatistics<PixelT> StatisticsVisitor<PixelT>::getStatistics() const
    {
        return _statistics;
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::getHistogramData(int *numPoints,
                                                     std::shared_ptr<uint32_t[]> *data)
    {
        *numPoints = PixUtils::g_histogramPoints;
        *data = _histogram;
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::pixelFormat(ELS::PixelFormat pf)
    {
        int totalHistogramPoints = PixUtils::g_histogramPoints;
        _isColor = pf != ELS::PF_GRAY;

        _isFirstPixel = true;
        _accumulator[0] = 0;
        _accumulator[1] = 0;
        _accumulator[2] = 0;
        _pixelCount = 0;

        if (_isColor)
        {
            totalHistogramPoints *= 3;
        }

        _histogram = std::shared_ptr<uint32_t[]>(new uint32_t[totalHistogramPoints]);
        for (int i = 0; i < PixUtils::g_histogramPoints; i++)
        {
            _histogram[i] = 0;
            if (_isColor)
            {
                _histogram[PixUtils::g_histogramPoints + i] = 0;
                _histogram[PixUtils::g_histogramPoints * 2 + i] = 0;
            }
        }
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::dimensions(int width, int height)
    {
        _width = width;
        (void)height;
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::rowInfo(int stride)
    {
        _stride = stride;
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::rowGray(int y,
                                            const PixelT *k)
    {
        (void)y;
        for (int i = 0, dataIdx = 0; i < _width; i++, dataIdx += _stride)
        {
            _pixelCount++;

            if (_isFirstPixel)
            {
                _isFirstPixel = false;
                _minVal[0] = k[dataIdx];
                _maxVal[0] = k[dataIdx];
            }
            else
            {
                if (k[dataIdx] < _minVal[0])
                {
                    _minVal[0] = k[dataIdx];
                }
                if (k[dataIdx] > _maxVal[0])
                {
                    _maxVal[0] = k[dataIdx];
                }
            }

            _accumulator[0] += k[dataIdx];

            _histogram[PixUtils::convertRangeToHist(k[dataIdx])]++;
        }
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::rowRgb(int y,
                                           const PixelT *r,
                                           const PixelT *g,
                                           const PixelT *b)
    {
        (void)y;
        for (int i = 0, dataIdx = 0; i < _width; i++, dataIdx += _stride)
        {
            _pixelCount++;

            if (_isFirstPixel)
            {
                _isFirstPixel = false;
                _minVal[0] = r[dataIdx];
                _minVal[1] = g[dataIdx];
                _minVal[2] = b[dataIdx];
                _maxVal[0] = r[dataIdx];
                _maxVal[1] = g[dataIdx];
                _maxVal[2] = b[dataIdx];
            }
            else
            {
                if (r[dataIdx] < _minVal[0])
                {
                    _minVal[0] = r[dataIdx];
                }
                if (g[dataIdx] < _minVal[1])
                {
                    _minVal[1] = g[dataIdx];
                }
                if (b[dataIdx] < _minVal[2])
                {
                    _minVal[2] = b[dataIdx];
                }
                if (r[dataIdx] > _maxVal[0])
                {
                    _maxVal[0] = r[dataIdx];
                }
                if (g[dataIdx] > _maxVal[1])
                {
                    _maxVal[1] = g[dataIdx];
                }
                if (b[dataIdx] > _maxVal[2])
                {
                    _maxVal[2] = b[dataIdx];
                }

                _accumulator[0] += r[dataIdx];
                _accumulator[1] += g[dataIdx];
                _accumulator[2] += b[dataIdx];

                _histogram[PixUtils::convertRangeToHist(r[dataIdx])]++;
                _histogram[PixUtils::g_histogramPoints + PixUtils::convertRangeToHist(g[dataIdx])]++;
                _histogram[PixUtils::g_histogramPoints * 2 + PixUtils::convertRangeToHist(b[dataIdx])]++;
            }
        }
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::done()
    {
        const int gOffset = PixUtils::g_histogramPoints;
        const int bOffset = PixUtils::g_histogramPoints * 2;

        _statistics.setMinVal(0, _minVal[0]);
        _statistics.setMaxVal(0, _maxVal[0]);
        _statistics.setMeanVal(0, (PixelT)(_accumulator[0] / _pixelCount));
        if (_isColor)
        {
            _statistics.setMinVal(1, _minVal[1]);
            _statistics.setMinVal(2, _minVal[2]);
            _statistics.setMaxVal(1, _maxVal[1]);
            _statistics.setMaxVal(2, _maxVal[2]);
            _statistics.setMeanVal(1, (PixelT)(_accumulator[1] / _pixelCount));
            _statistics.setMeanVal(2, (PixelT)(_accumulator[2] / _pixelCount));
        }

        int64_t halfway = _pixelCount / 2;
        int64_t pointCount[3] = {0, 0, 0};
        uint16_t medHist[3] = {0, 0, 0};
        bool done = false;
        for (uint16_t i = 0; (!done) && (i < PixUtils::g_histogramPoints); i++)
        {
            done = true;

            if (pointCount[0] < halfway)
            {
                pointCount[0] += _histogram[i];
                if (pointCount[0] >= halfway)
                {
                    PixelT val = 0;
                    PixUtils::convertRangeFromHist(i, &val);
                    medHist[0] = i;
                    _statistics.setMedVal(0, val);
                }
                else
                {
                    done = false;
                }
            }

            if (_isColor)
            {
                if (pointCount[1] < halfway)
                {
                    pointCount[1] += _histogram[gOffset + i];
                    if (pointCount[1] >= halfway)
                    {
                        PixelT val = 0;
                        PixUtils::convertRangeFromHist(i, &val);
                        medHist[1] = i;
                        _statistics.setMedVal(1, val);
                    }
                    else
                    {
                        done = false;
                    }
                }

                if (pointCount[2] < halfway)
                {
                    pointCount[2] += _histogram[bOffset + i];
                    if (pointCount[2] >= halfway)
                    {
                        PixelT val = 0;
                        PixUtils::convertRangeFromHist(i, &val);
                        medHist[2] = i;
                        _statistics.setMedVal(2, val);
                    }
                    else
                    {
                        done = false;
                    }
                }
            }
        }

        int totalHistogramPoints = _isColor ? PixUtils::g_histogramPoints * 3 : PixUtils::g_histogramPoints;
        uint32_t *tmp = new uint32_t[totalHistogramPoints];
        for (int i = 0; i < totalHistogramPoints; i++)
        {
            tmp[i] = 0;
        }
        for (uint16_t i = 0; i < PixUtils::g_histogramPoints; i++)
        {
            uint16_t val = i > medHist[0] ? i - medHist[0] : medHist[0] - i;
            tmp[val] += _histogram[i];

            if (_isColor)
            {
                val = i > medHist[1] ? i - medHist[1] : medHist[1] - i;
                tmp[gOffset + val] += _histogram[gOffset + i];

                val = i > medHist[2] ? i - medHist[2] : medHist[2] - i;
                tmp[bOffset + val] += _histogram[bOffset + i];
            }
        }

        pointCount[0] = 0;
        pointCount[1] = 0;
        pointCount[2] = 0;
        done = false;
        for (uint16_t i = 0; (!done) && (i < PixUtils::g_histogramPoints); i++)
        {
            done = true;

            if (pointCount[0] < halfway)
            {
                pointCount[0] += tmp[i];
                if (pointCount[0] >= halfway)
                {
                    PixelT val = 0;
                    PixUtils::convertRangeFromHist(i, &val);
                    _statistics.setMADN(0, (PixelT)(PixUtils::g_madnConstant * (double)val));
                }
                else
                {
                    done = false;
                }
            }

            if (_isColor)
            {
                if (pointCount[1] < halfway)
                {
                    pointCount[1] += tmp[gOffset + i];
                    if (pointCount[1] >= halfway)
                    {
                        PixelT val = 0;
                        PixUtils::convertRangeFromHist(i, &val);
                        _statistics.setMADN(1, (PixelT)(PixUtils::g_madnConstant * (double)val));
                    }
                    else
                    {
                        done = false;
                    }
                }

                if (pointCount[2] < halfway)
                {
                    pointCount[2] += tmp[bOffset + i];
                    if (pointCount[2] >= halfway)
                    {
                        PixelT val = 0;
                        PixUtils::convertRangeFromHist(i, &val);
                        _statistics.setMADN(2, (PixelT)(PixUtils::g_madnConstant * (double)val));
                    }
                    else
                    {
                        done = false;
                    }
                }
            }
        }

        delete[] tmp;
    }

}