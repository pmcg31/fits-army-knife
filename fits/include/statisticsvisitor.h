#pragma once

#include <memory>
#include <inttypes.h>
#include "pixutils.h"
#include "pixstatistics.h"
#include "fitspixelvisitor.h"

namespace ELS
{

    template <typename PixelT>
    class StatisticsVisitor : public ELS::FITSPixelVisitor<PixelT>
    {
    public:
        StatisticsVisitor();
        ~StatisticsVisitor();

        PixStatistics<PixelT> getStatistics() const;

        void getHistogramData(int *numPoints,
                              std::shared_ptr<uint32_t[]> *data);

    public:
        virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void pixel(int x, int y, const PixelT *pixelVals) override;
        virtual void done() override;

    private:
        int _width;
        int _height;
        int _rIdx;
        int _gIdx;
        int _bIdx;
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
        : _width(0),
          _height(0),
          _rIdx(0),
          _gIdx(-1),
          _bIdx(-1),
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
    void StatisticsVisitor<PixelT>::pixelFormat(ELS::FITS::PixelFormat pf)
    {
        int totalHistogramPoints = PixUtils::g_histogramPoints;
        bool isColor = true;
        switch (pf)
        {
        case ELS::FITS::PF_GRAY:
            isColor = false;
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

        _isFirstPixel = true;
        _accumulator[0] = 0;
        _accumulator[1] = 0;
        _accumulator[2] = 0;
        _pixelCount = 0;

        if (isColor)
        {
            totalHistogramPoints *= 3;
        }

        _histogram = std::shared_ptr<uint32_t[]>(new uint32_t[totalHistogramPoints]);
        // _histogram = new uint32_t[totalHistogramPoints];
        for (int i = 0; i < PixUtils::g_histogramPoints; i++)
        {
            _histogram[i] = 0;
            if (isColor)
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
        _height = height;
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::pixel(int x, int y, const PixelT *pixelVals)
    {
        (void)x;
        (void)y;

        _pixelCount++;

        if (_gIdx != -1)
        {
            // Color
            if (_isFirstPixel)
            {
                _isFirstPixel = false;
                _minVal[0] = pixelVals[_rIdx];
                _minVal[1] = pixelVals[_gIdx];
                _minVal[2] = pixelVals[_bIdx];
                _maxVal[0] = pixelVals[_rIdx];
                _maxVal[1] = pixelVals[_gIdx];
                _maxVal[2] = pixelVals[_bIdx];
            }
            else
            {
                if (pixelVals[_rIdx] < _minVal[0])
                {
                    _minVal[0] = pixelVals[_rIdx];
                }
                if (pixelVals[_gIdx] < _minVal[1])
                {
                    _minVal[1] = pixelVals[_gIdx];
                }
                if (pixelVals[_bIdx] < _minVal[2])
                {
                    _minVal[2] = pixelVals[_bIdx];
                }
                if (pixelVals[_rIdx] > _maxVal[0])
                {
                    _maxVal[0] = pixelVals[_rIdx];
                }
                if (pixelVals[_gIdx] > _maxVal[1])
                {
                    _maxVal[1] = pixelVals[_gIdx];
                }
                if (pixelVals[_bIdx] > _maxVal[2])
                {
                    _maxVal[2] = pixelVals[_bIdx];
                }

                _accumulator[0] += pixelVals[_rIdx];
                _accumulator[1] += pixelVals[_gIdx];
                _accumulator[2] += pixelVals[_bIdx];

                _histogram[PixUtils::convertRangeToHist(pixelVals[_rIdx])]++;
                _histogram[PixUtils::g_histogramPoints + PixUtils::convertRangeToHist(pixelVals[_gIdx])]++;
                _histogram[PixUtils::g_histogramPoints * 2 + PixUtils::convertRangeToHist(pixelVals[_bIdx])]++;
            }
        }
        else
        {
            // Grayscale
            if (_isFirstPixel)
            {
                _isFirstPixel = false;
                _minVal[0] = pixelVals[0];
                _maxVal[0] = pixelVals[0];
            }
            else
            {
                if (pixelVals[0] < _minVal[0])
                {
                    _minVal[0] = pixelVals[0];
                }
                if (pixelVals[0] > _maxVal[0])
                {
                    _maxVal[0] = pixelVals[0];
                }
            }

            _accumulator[0] += pixelVals[0];

            _histogram[PixUtils::convertRangeToHist(pixelVals[0])]++;
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
        if (_gIdx != -1)
        {
            _statistics.setMinVal(1, _minVal[1]);
            _statistics.setMinVal(2, _minVal[2]);
            _statistics.setMaxVal(1, _maxVal[1]);
            _statistics.setMaxVal(2, _maxVal[2]);
            _statistics.setMeanVal(1, (PixelT)(_accumulator[1] / _pixelCount));
            _statistics.setMeanVal(2, (PixelT)(_accumulator[2] / _pixelCount));
        }

        bool isColor = _gIdx != -1;

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

            if (isColor)
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

        int totalHistogramPoints = isColor ? PixUtils::g_histogramPoints * 3 : PixUtils::g_histogramPoints;
        uint32_t *tmp = new uint32_t[totalHistogramPoints];
        for (int i = 0; i < totalHistogramPoints; i++)
        {
            tmp[i] = 0;
        }
        for (uint16_t i = 0; i < PixUtils::g_histogramPoints; i++)
        {
            uint16_t val = i > medHist[0] ? i - medHist[0] : medHist[0] - i;
            tmp[val] += _histogram[i];

            if (isColor)
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

            if (isColor)
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