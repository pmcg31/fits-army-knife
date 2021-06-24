#pragma once

#include <inttypes.h>
#include "fitspixelvisitor.h"

namespace ELS
{

    template <typename PixelT>
    class StatisticsVisitor : public ELS::FITSPixelVisitor<PixelT>
    {
    public:
        StatisticsVisitor();
        ~StatisticsVisitor();

        PixelT getMinVal(int chan = 0) const;
        PixelT getMaxVal(int chan = 0) const;
        PixelT getMedVal(int chan = 0) const;
        PixelT getMeanVal(int chan = 0) const;

        void getHistogramData(int *numPoints, uint32_t **data);

    public:
        virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void pixel(int x, int y, const PixelT *pixelVals) override;
        virtual void done() override;

    private:
        static uint16_t convertRangeToHist(uint8_t val);
        static uint16_t convertRangeToHist(uint16_t val);
        static uint16_t convertRangeToHist(uint32_t val);
        static uint16_t convertRangeToHist(float val);
        static uint16_t convertRangeToHist(double val);

        static void convertRangeFromHist(uint16_t hist, uint8_t *val);
        static void convertRangeFromHist(uint16_t hist, uint16_t *val);
        static void convertRangeFromHist(uint16_t hist, uint32_t *val);
        static void convertRangeFromHist(uint16_t hist, float *val);
        static void convertRangeFromHist(uint16_t hist, double *val);

    private:
        int _width;
        int _height;
        int _rIdx;
        int _gIdx;
        int _bIdx;
        bool _isFirstPixel;
        int64_t _pixelCount;
        uint32_t *_histogram;
        double _accumulator[3];
        PixelT _minVal[3];
        PixelT _maxVal[3];
        PixelT _medVal[3];
        PixelT _meanVal[3];
        bool _shouldDeleteHistogram;

    private:
        static const int g_histogramPoints = 65535;
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
          _medVal{0, 0, 0},
          _meanVal{0, 0, 0},
          _shouldDeleteHistogram(true)
    {
    }

    template <typename PixelT>
    StatisticsVisitor<PixelT>::~StatisticsVisitor()
    {
        if (_shouldDeleteHistogram && (_histogram != 0))
        {
            delete[] _histogram;
        }
    }

    template <typename PixelT>
    PixelT StatisticsVisitor<PixelT>::getMinVal(int chan /* = 0 */) const
    {
        return _minVal[chan];
    }

    template <typename PixelT>
    PixelT StatisticsVisitor<PixelT>::getMaxVal(int chan /* = 0 */) const
    {
        return _maxVal[chan];
    }

    template <typename PixelT>
    PixelT StatisticsVisitor<PixelT>::getMedVal(int chan /* = 0 */) const
    {
        return _medVal[chan];
    }

    template <typename PixelT>
    PixelT StatisticsVisitor<PixelT>::getMeanVal(int chan /* = 0 */) const
    {
        return _meanVal[chan];
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::getHistogramData(int *numPoints,
                                                     uint32_t **data)
    {
        _shouldDeleteHistogram = false;
        *numPoints = g_histogramPoints;
        *data = _histogram;
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::pixelFormat(ELS::FITS::PixelFormat pf)
    {
        int totalHistogramPoints = g_histogramPoints;
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

        _histogram = new uint32_t[totalHistogramPoints];
        for (int i = 0; i < g_histogramPoints; i++)
        {
            _histogram[i] = 0;
            if (isColor)
            {
                _histogram[g_histogramPoints + i] = 0;
                _histogram[g_histogramPoints * 2 + i] = 0;
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

                _histogram[convertRangeToHist(pixelVals[_rIdx])]++;
                _histogram[g_histogramPoints + convertRangeToHist(pixelVals[_gIdx])]++;
                _histogram[g_histogramPoints * 2 + convertRangeToHist(pixelVals[_bIdx])]++;
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

            _histogram[convertRangeToHist(pixelVals[0])]++;
        }
    }

    template <typename PixelT>
    void StatisticsVisitor<PixelT>::done()
    {
        _meanVal[0] = (PixelT)(_accumulator[0] / _pixelCount);
        if (_gIdx != -1)
        {
            _meanVal[1] = (PixelT)(_accumulator[1] / _pixelCount);
            _meanVal[2] = (PixelT)(_accumulator[2] / _pixelCount);
        }

        bool isColor = _gIdx != -1;

        int64_t halfway = _pixelCount / 2;
        int64_t pointCount[3] = {0, 0, 0};
        bool done = false;
        for (int i = 0; (!done) && (i < g_histogramPoints); i++)
        {
            done = true;

            if (pointCount[0] < halfway)
            {
                pointCount[0] += _histogram[i];
                if (pointCount[0] >= halfway)
                {
                    PixelT val = 0;
                    convertRangeFromHist(i, &val);
                    _medVal[0] = val;
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
                    pointCount[1] += _histogram[g_histogramPoints + i];
                    if (pointCount[1] >= halfway)
                    {
                        PixelT val = 0;
                        convertRangeFromHist(i, &val);
                        _medVal[1] = val;
                    }
                    else
                    {
                        done = false;
                    }
                }

                if (pointCount[2] < halfway)
                {
                    pointCount[2] += _histogram[g_histogramPoints * 2 + i];
                    if (pointCount[2] >= halfway)
                    {
                        PixelT val = 0;
                        convertRangeFromHist(i, &val);
                        _medVal[2] = val;
                    }
                    else
                    {
                        done = false;
                    }
                }
            }
        }
    }

    /* static */
    template <typename PixelT>
    uint16_t StatisticsVisitor<PixelT>::convertRangeToHist(uint8_t val)
    {
        uint8_t factor = 255 / g_histogramPoints;

        uint16_t newVal = (uint16_t)(val / factor);
        if (!(newVal < g_histogramPoints))
        {
            // printf("Fixed newVal of %d to %d\n", newVal, g_histogramPoints - 1);
            // fflush(stdout);
            newVal = g_histogramPoints - 1;
        }

        return newVal;
    }

    /* static */
    template <typename PixelT>
    uint16_t StatisticsVisitor<PixelT>::convertRangeToHist(uint16_t val)
    {
        uint16_t factor = 65535 / g_histogramPoints;

        uint16_t newVal = val / factor;
        if (!(newVal < g_histogramPoints))
        {
            // printf("Fixed newVal of %d to %d\n", newVal, g_histogramPoints - 1);
            // fflush(stdout);
            newVal = g_histogramPoints - 1;
        }

        return newVal;
    }

    /* static */
    template <typename PixelT>
    uint16_t StatisticsVisitor<PixelT>::convertRangeToHist(uint32_t val)
    {
        uint32_t factor = 4294967295 / g_histogramPoints;

        uint16_t newVal = (uint16_t)(val / factor);
        if (!(newVal < g_histogramPoints))
        {
            // printf("Fixed newVal of %d to %d\n", newVal, g_histogramPoints - 1);
            // fflush(stdout);
            newVal = g_histogramPoints - 1;
        }

        return newVal;
    }

    /* static */
    template <typename PixelT>
    uint16_t StatisticsVisitor<PixelT>::convertRangeToHist(float val)
    {
        uint16_t newVal = (uint16_t)(val * g_histogramPoints);
        if (!(newVal < g_histogramPoints))
        {
            // printf("Fixed newVal of %d to %d\n", newVal, g_histogramPoints - 1);
            // fflush(stdout);
            newVal = g_histogramPoints - 1;
        }

        return newVal;
    }

    /* static */
    template <typename PixelT>
    uint16_t StatisticsVisitor<PixelT>::convertRangeToHist(double val)
    {
        uint16_t newVal = (uint16_t)(val * g_histogramPoints);
        if (!(newVal < g_histogramPoints))
        {
            // printf("Fixed newVal of %d to %d\n", newVal, g_histogramPoints - 1);
            // fflush(stdout);
            newVal = g_histogramPoints - 1;
        }

        return newVal;
    }

    /* static */
    template <typename PixelT>
    void StatisticsVisitor<PixelT>::convertRangeFromHist(uint16_t hist, uint8_t *val)
    {
        uint8_t factor = 255 / g_histogramPoints;

        *val = (uint8_t)(hist / factor);
    }

    /* static */
    template <typename PixelT>
    void StatisticsVisitor<PixelT>::convertRangeFromHist(uint16_t hist, uint16_t *val)
    {
        uint16_t factor = 65535 / g_histogramPoints;

        *val = (uint16_t)(hist / factor);
    }

    /* static */
    template <typename PixelT>
    void StatisticsVisitor<PixelT>::convertRangeFromHist(uint16_t hist, uint32_t *val)
    {
        uint32_t factor = 4294967295 / g_histogramPoints;

        *val = (uint32_t)(hist / factor);
    }

    /* static */
    template <typename PixelT>
    void StatisticsVisitor<PixelT>::convertRangeFromHist(uint16_t hist, float *val)
    {
        *val = (float)hist / g_histogramPoints;
    }

    /* static */
    template <typename PixelT>
    void StatisticsVisitor<PixelT>::convertRangeFromHist(uint16_t hist, double *val)
    {
        *val = (double)hist / g_histogramPoints;
    }

}