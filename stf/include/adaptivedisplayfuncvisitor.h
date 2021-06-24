#pragma once

#include <algorithm>
#include <inttypes.h>
#include "fitspixelvisitor.h"

namespace ELS
{

    template <typename PixelT>
    class AdaptiveDisplayFuncVisitor : public ELS::FITSPixelVisitor<PixelT>
    {
    public:
        AdaptiveDisplayFuncVisitor(PixelT medianVals[3]);
        ~AdaptiveDisplayFuncVisitor();

    public:
        virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void pixel(int x, int y, const PixelT *pixelVals) override;
        virtual void done() override;

    private:
        static double midtonesTransferFunc(double x, double m);

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
        PixelT _medVal[3];
        int _rIdx;
        int _gIdx;
        int _bIdx;
        int64_t _pixelCount;
        uint32_t *_histogram;
        double _madn[3];
        double _sClip[3];
        double _hClip[3];
        double _mBal[3];

    private:
        static const int g_histogramPoints = 65535;
    };

    template <typename PixelT>
    AdaptiveDisplayFuncVisitor<PixelT>::AdaptiveDisplayFuncVisitor(PixelT medianVals[3])
        : _medVal{medianVals[0], medianVals[1], medianVals[2]},
          _rIdx(0),
          _gIdx(-1),
          _bIdx(-1),
          _pixelCount(0),
          _histogram(0),
          _madn{0, 0, 0},
          _sClip{0, 0, 0},
          _hClip{0, 0, 0},
          _mBal{0, 0, 0}
    {
    }

    template <typename PixelT>
    AdaptiveDisplayFuncVisitor<PixelT>::~AdaptiveDisplayFuncVisitor()
    {
        if (_histogram != 0)
        {
            delete[] _histogram;
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::pixelFormat(ELS::FITS::PixelFormat pf)
    {
        switch (pf)
        {
        case ELS::FITS::PF_GRAY:
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

        _pixelCount = 0;

        int totalHistogramPoints = g_histogramPoints;
        if (_gIdx != -1)
        {
            totalHistogramPoints *= 3;
        }

        _histogram = new uint32_t[totalHistogramPoints];
        for (int i = 0; i < g_histogramPoints; i++)
        {
            _histogram[i] = 0;
            if (_gIdx != -1)
            {
                _histogram[g_histogramPoints + i] = 0;
                _histogram[g_histogramPoints * 2 + i] = 0;
            }
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::dimensions(int width, int height)
    {
        (void)width;
        (void)height;
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::pixel(int x, int y, const PixelT *pixelVals)
    {
        (void)x;
        (void)y;

        _pixelCount++;

        if (_gIdx != -1)
        {
            // Color
            PixelT rVal = pixelVals[_rIdx] > _medVal[0] ? pixelVals[_rIdx] - _medVal[0] : _medVal[0] - pixelVals[_rIdx];
            PixelT gVal = pixelVals[_gIdx] > _medVal[1] ? pixelVals[_gIdx] - _medVal[1] : _medVal[1] - pixelVals[_gIdx];
            PixelT bVal = pixelVals[_bIdx] > _medVal[2] ? pixelVals[_bIdx] - _medVal[2] : _medVal[2] - pixelVals[_bIdx];

            _histogram[convertRangeToHist(rVal)]++;
            _histogram[g_histogramPoints + convertRangeToHist(gVal)]++;
            _histogram[g_histogramPoints * 2 + convertRangeToHist(bVal)]++;
        }
        else
        {
            PixelT val = pixelVals[0] > _medVal[0] ? pixelVals[0] - _medVal[0] : _medVal[0] - pixelVals[0];

            _histogram[convertRangeToHist(val)]++;
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::done()
    {
        const double madnConstant = 1.4826;
        const double B = 0.25;
        const double C = -2.8;

        int numChan = _gIdx != -1 ? 3 : 1;

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
                    double dval = 0.0;
                    convertRangeFromHist(i, &dval);
                    _madn[0] = dval * madnConstant;
                }
                else
                {
                    done = false;
                }
            }

            if (numChan == 3)
            {
                if (pointCount[1] < halfway)
                {
                    pointCount[1] += _histogram[g_histogramPoints + i];
                    if (pointCount[1] >= halfway)
                    {
                        double dval = 0.0;
                        convertRangeFromHist(i, &dval);
                        _madn[1] = dval * madnConstant;
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
                        double dval = 0.0;
                        convertRangeFromHist(i, &dval);
                        _madn[2] = dval * madnConstant;
                    }
                    else
                    {
                        done = false;
                    }
                }
            }
        }

        for (int chan = 0; chan < numChan; chan++)
        {
            uint16_t tmp = convertRangeToHist(_medVal[chan]);
            double medVal = 0.0;
            convertRangeFromHist(tmp, &medVal);

            if ((medVal > 0.5) || (_madn[chan] == 0))
            {
                _sClip[chan] = 0;
            }
            else
            {
                _sClip[chan] = std::min(1.0, std::max(0.0, medVal + C * _madn[chan]));
            }
            if ((medVal < 0.5) || (_madn[chan] == 0))
            {
                _hClip[chan] = 1;
            }
            else
            {
                _hClip[chan] = std::min(1.0, std::max(0.0, medVal - C * _madn[chan]));
            }
            if (medVal < 0.5)
            {
                _mBal[chan] = midtonesTransferFunc(medVal - _sClip[chan], B);
            }
            else
            {
                _mBal[chan] = midtonesTransferFunc(B, _hClip[chan] - medVal);
            }
        }

        for (int chan = 0; chan < numChan; chan++)
        {
            printf("c%d: madn: %0.4f sClip: %0.4f hClip: %0.4f mBal: %0.4f\n",
                   chan, _madn[chan], _sClip[chan], _hClip[chan], _mBal[chan]);
        }
        fflush(stdout);
    }

    /* static */
    template <typename PixelT>
    double AdaptiveDisplayFuncVisitor<PixelT>::midtonesTransferFunc(double x,
                                                                    double m)
    {
        if (x == 0)
        {
            return 0.0;
        }
        if (x == m)
        {
            return 0.5;
        }
        if (x == 1)
        {
            return 1.0;
        }

        return ((m - 1) * x) / (((2 * m - 1) * x) - m);
    }

    /* static */
    template <typename PixelT>
    uint16_t AdaptiveDisplayFuncVisitor<PixelT>::convertRangeToHist(uint8_t val)
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
    uint16_t AdaptiveDisplayFuncVisitor<PixelT>::convertRangeToHist(uint16_t val)
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
    uint16_t AdaptiveDisplayFuncVisitor<PixelT>::convertRangeToHist(uint32_t val)
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
    uint16_t AdaptiveDisplayFuncVisitor<PixelT>::convertRangeToHist(float val)
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
    uint16_t AdaptiveDisplayFuncVisitor<PixelT>::convertRangeToHist(double val)
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
    void AdaptiveDisplayFuncVisitor<PixelT>::convertRangeFromHist(uint16_t hist, uint8_t *val)
    {
        uint8_t factor = 255 / g_histogramPoints;

        *val = (uint8_t)(hist / factor);
    }

    /* static */
    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::convertRangeFromHist(uint16_t hist, uint16_t *val)
    {
        uint16_t factor = 65535 / g_histogramPoints;

        *val = (uint16_t)(hist / factor);
    }

    /* static */
    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::convertRangeFromHist(uint16_t hist, uint32_t *val)
    {
        uint32_t factor = 4294967295 / g_histogramPoints;

        *val = (uint32_t)(hist / factor);
    }

    /* static */
    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::convertRangeFromHist(uint16_t hist, float *val)
    {
        *val = (float)hist / g_histogramPoints;
    }

    /* static */
    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::convertRangeFromHist(uint16_t hist, double *val)
    {
        *val = (double)hist / g_histogramPoints;
    }

}