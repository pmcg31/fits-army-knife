#include "pixutils.h"

namespace ELS
{

    const int PixUtils::g_histogramPoints = 65535;

    const double PixUtils::g_madnConstant = 1.4826;

    const uint8_t PixUtils::g_u8Max = 255;
    const uint8_t PixUtils::g_u8Mid = 127;

    const uint16_t PixUtils::g_u16Max = 65535;
    const uint16_t PixUtils::g_u16Mid = 32767;

    const uint32_t PixUtils::g_u32Max = 4294967295;
    const uint32_t PixUtils::g_u32Mid = 2147483647;

    /* static */
    double PixUtils::midtonesTransferFunc(uint8_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel / PixUtils::g_u8Max;
        if (tmpPix == 0)
        {
            return 0;
        }

        if (tmpPix == mBal)
        {
            return g_u8Mid;
        }

        if (tmpPix == 1)
        {
            return g_u8Max;
        }

        return ((mBal - 1) * tmpPix) / (((2 * mBal - 1) * tmpPix) - mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(uint16_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;
        if (tmpPix == 0)
        {
            return 0;
        }

        if (tmpPix == mBal)
        {
            return g_u16Mid;
        }

        if (tmpPix == 1)
        {
            return g_u16Max;
        }

        return ((mBal - 1) * tmpPix) / (((2 * mBal - 1) * tmpPix) - mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(uint32_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel / PixUtils::g_u32Max;
        if (tmpPix == 0)
        {
            return 0;
        }

        if (tmpPix == mBal)
        {
            return g_u32Mid;
        }

        if (tmpPix == 1)
        {
            return g_u32Max;
        }

        return ((mBal - 1) * tmpPix) / (((2 * mBal - 1) * tmpPix) - mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(float pixel,
                                          double mBal)
    {
        if (pixel == 0)
        {
            return 0.0;
        }

        if (pixel == mBal)
        {
            return 0.5;
        }

        if (pixel == 1)
        {
            return 1.0;
        }

        return ((mBal - 1) * pixel) / (((2 * mBal - 1) * pixel) - mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(double pixel,
                                          double mBal)
    {
        if (pixel == 0)
        {
            return 0.0;
        }

        if (pixel == mBal)
        {
            return 0.5;
        }

        if (pixel == 1)
        {
            return 1.0;
        }

        return ((mBal - 1) * pixel) / (((2 * mBal - 1) * pixel) - mBal);
    }

    /* static */
    double PixUtils::clippingFunc(uint8_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)pixel / PixUtils::g_u8Max;
        if (tmpPix < sClip)
        {
            return 0;
        }

        if (tmpPix > hClip)
        {
            return g_u8Max;
        }

        return (tmpPix - sClip) / (hClip - sClip);
    }

    /* static */
    double PixUtils::clippingFunc(uint16_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;
        if (tmpPix < sClip)
        {
            return 0;
        }

        if (tmpPix > hClip)
        {
            return g_u16Max;
        }

        return (tmpPix - sClip) / (hClip - sClip);
    }

    /* static */
    double PixUtils::clippingFunc(uint32_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;
        if (tmpPix < sClip)
        {
            return 0;
        }

        if (tmpPix > hClip)
        {
            return g_u32Max;
        }

        return (tmpPix - sClip) / (hClip - sClip);
    }

    /* static */
    double PixUtils::clippingFunc(float pixel, double sClip, double hClip)
    {
        if (pixel < sClip)
        {
            return 0.0;
        }

        if (pixel > hClip)
        {
            return 1.0;
        }

        return (pixel - sClip) / (hClip - sClip);
    }

    /* static */
    double PixUtils::clippingFunc(double pixel, double sClip, double hClip)
    {
        if (pixel < sClip)
        {
            return 0.0;
        }

        if (pixel > hClip)
        {
            return 1.0;
        }

        return (pixel - sClip) / (hClip - sClip);
    }

    /* static */
    double PixUtils::expansionFunc(uint8_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)pixel / PixUtils::g_u8Max;
        return (tmpPix - sExp) / (hExp - sExp);
    }

    /* static */
    double PixUtils::expansionFunc(uint16_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;
        return (tmpPix - sExp) / (hExp - sExp);
    }

    /* static */
    double PixUtils::expansionFunc(uint32_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)pixel / PixUtils::g_u32Max;
        return (tmpPix - sExp) / (hExp - sExp);
    }

    /* static */
    double PixUtils::expansionFunc(float pixel, double sExp, double hExp)
    {
        return (pixel - sExp) / (hExp - sExp);
    }

    /* static */
    double PixUtils::expansionFunc(double pixel, double sExp, double hExp)
    {
        return (pixel - sExp) / (hExp - sExp);
    }

    /* static */
    uint16_t PixUtils::convertRangeToHist(uint8_t val)
    {
        uint8_t factor = g_u8Max / g_histogramPoints;

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
    uint16_t PixUtils::convertRangeToHist(uint16_t val)
    {
        uint16_t factor = g_u16Max / g_histogramPoints;

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
    uint16_t PixUtils::convertRangeToHist(uint32_t val)
    {
        uint32_t factor = g_u32Max / g_histogramPoints;

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
    uint16_t PixUtils::convertRangeToHist(float val)
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
    uint16_t PixUtils::convertRangeToHist(double val)
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
    void PixUtils::convertRangeFromHist(uint16_t hist, uint8_t *val)
    {
        uint8_t factor = g_u8Max / g_histogramPoints;

        *val = (uint8_t)(hist / factor);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, uint16_t *val)
    {
        uint16_t factor = g_u16Max / g_histogramPoints;

        *val = (uint16_t)(hist / factor);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, uint32_t *val)
    {
        uint32_t factor = g_u32Max / g_histogramPoints;

        *val = (uint32_t)(hist / factor);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, float *val)
    {
        *val = (float)hist / g_histogramPoints;
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, double *val)
    {
        *val = (double)hist / g_histogramPoints;
    }

}