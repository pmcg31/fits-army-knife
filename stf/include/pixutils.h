#pragma once

#include <inttypes.h>

namespace ELS
{

    class PixUtils
    {
    public:
        static double midtonesTransferFunc(double pixel, double mBal);
        static double clippingFunc(double pixel, double sClip, double hClip);
        static double expansionFunc(double pixel, double sExp, double hExp);
        static double screenTransferFunc(double pixel,
                                         double mBal,
                                         double sClip,
                                         double hClip,
                                         double sExp,
                                         double hExp);

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

    public:
        static const int g_histogramPoints = 65535;
    };

}