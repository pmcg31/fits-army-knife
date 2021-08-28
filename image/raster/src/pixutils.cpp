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
    double PixUtils::midtonesTransferFunc(int8_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u8Mid + 1) / PixUtils::g_u8Max;
        return midtonesTransferFunc(tmpPix, mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(int16_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u16Mid + 1) / PixUtils::g_u16Max;
        return midtonesTransferFunc(tmpPix, mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(int32_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u32Mid + 1) / PixUtils::g_u32Max;
        return midtonesTransferFunc(tmpPix, mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(uint8_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel / PixUtils::g_u8Max;
        return midtonesTransferFunc(tmpPix, mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(uint16_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;
        return midtonesTransferFunc(tmpPix, mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(uint32_t pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel / PixUtils::g_u32Max;
        return midtonesTransferFunc(tmpPix, mBal);
    }

    /* static */
    double PixUtils::midtonesTransferFunc(float pixel,
                                          double mBal)
    {
        double tmpPix = (double)pixel;
        return midtonesTransferFunc(tmpPix, mBal);
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
    double PixUtils::clippingFunc(int8_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u8Mid + 1) / PixUtils::g_u8Max;

        return clippingFunc(tmpPix, sClip, hClip);
    }

    /* static */
    double PixUtils::clippingFunc(int16_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u16Mid + 1) / PixUtils::g_u16Max;

        return clippingFunc(tmpPix, sClip, hClip);
    }

    /* static */
    double PixUtils::clippingFunc(int32_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u32Mid + 1) / PixUtils::g_u16Max;

        return clippingFunc(tmpPix, sClip, hClip);
    }

    /* static */
    double PixUtils::clippingFunc(uint8_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)pixel / PixUtils::g_u8Max;

        return clippingFunc(tmpPix, sClip, hClip);
    }

    /* static */
    double PixUtils::clippingFunc(uint16_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;

        return clippingFunc(tmpPix, sClip, hClip);
    }

    /* static */
    double PixUtils::clippingFunc(uint32_t pixel, double sClip, double hClip)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;

        return clippingFunc(tmpPix, sClip, hClip);
    }

    /* static */
    double PixUtils::clippingFunc(float pixel, double sClip, double hClip)
    {
        double tmpPix = pixel;
        return clippingFunc(tmpPix, sClip, hClip);
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
    double PixUtils::expansionFunc(int8_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u8Mid + 1) / PixUtils::g_u8Max;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(int16_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u16Mid + 1) / PixUtils::g_u16Max;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(int32_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)(pixel + PixUtils::g_u32Mid + 1) / PixUtils::g_u32Max;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(uint8_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)pixel / PixUtils::g_u8Max;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(uint16_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)pixel / PixUtils::g_u16Max;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(uint32_t pixel, double sExp, double hExp)
    {
        double tmpPix = (double)pixel / PixUtils::g_u32Max;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(float pixel, double sExp, double hExp)
    {
        double tmpPix = pixel;
        return expansionFunc(tmpPix, sExp, hExp);
    }

    /* static */
    double PixUtils::expansionFunc(double pixel, double sExp, double hExp)
    {
        return (pixel - sExp) / (hExp - sExp);
    }

    /* static */
    uint16_t PixUtils::convertRangeToHist(int8_t val)
    {
        return convertRangeToHist((uint8_t)val + PixUtils::g_u8Mid + 1);
    }

    /* static */
    uint16_t PixUtils::convertRangeToHist(int16_t val)
    {
        return convertRangeToHist((uint16_t)val + PixUtils::g_u16Mid + 1);
    }

    /* static */
    uint16_t PixUtils::convertRangeToHist(int32_t val)
    {
        return convertRangeToHist((uint32_t)val + PixUtils::g_u32Mid + 1);
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
    void PixUtils::convertRangeFromHist(uint16_t hist, int8_t *val)
    {
        uint8_t factor = g_u8Max / g_histogramPoints;

        *val = (int8_t)(((int32_t)hist / factor) - g_u8Mid - 1);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, int16_t *val)
    {
        uint16_t factor = g_u16Max / g_histogramPoints;

        *val = (int16_t)(((int32_t)hist / factor) - g_u16Mid - 1);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, int32_t *val)
    {
        uint32_t factor = g_u32Max / g_histogramPoints;

        *val = (int32_t)(((int64_t)hist / factor) - g_u32Mid - 1);
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