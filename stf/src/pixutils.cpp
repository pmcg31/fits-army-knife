#include "pixutils.h"

namespace ELS
{

    /* static */
    double PixUtils::midtonesTransferFunc(double x,
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
    uint16_t PixUtils::convertRangeToHist(uint8_t val)
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
    uint16_t PixUtils::convertRangeToHist(uint16_t val)
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
    uint16_t PixUtils::convertRangeToHist(uint32_t val)
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
        uint8_t factor = 255 / g_histogramPoints;

        *val = (uint8_t)(hist / factor);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, uint16_t *val)
    {
        uint16_t factor = 65535 / g_histogramPoints;

        *val = (uint16_t)(hist / factor);
    }

    /* static */
    void PixUtils::convertRangeFromHist(uint16_t hist, uint32_t *val)
    {
        uint32_t factor = 4294967295 / g_histogramPoints;

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