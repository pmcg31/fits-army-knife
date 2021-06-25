#pragma once

#include <inttypes.h>

namespace ELS
{

    class PixUtils
    {
    public:
        static double midtonesTransferFunc(uint8_t pixel, double mBal);
        static double midtonesTransferFunc(uint16_t pixel, double mBal);
        static double midtonesTransferFunc(uint32_t pixel, double mBal);
        static double midtonesTransferFunc(float pixel, double mBal);
        static double midtonesTransferFunc(double pixel, double mBal);

        static double clippingFunc(uint8_t pixel, double sClip, double hClip);
        static double clippingFunc(uint16_t pixel, double sClip, double hClip);
        static double clippingFunc(uint32_t pixel, double sClip, double hClip);
        static double clippingFunc(float pixel, double sClip, double hClip);
        static double clippingFunc(double pixel, double sClip, double hClip);

        static double expansionFunc(uint8_t pixel, double sExp, double hExp);
        static double expansionFunc(uint16_t pixel, double sExp, double hExp);
        static double expansionFunc(uint32_t pixel, double sExp, double hExp);
        static double expansionFunc(float pixel, double sExp, double hExp);
        static double expansionFunc(double pixel, double sExp, double hExp);

        template <typename PixelT>
        static double screenTransferFunc(PixelT pixel,
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
        static const int g_histogramPoints;

        static const double g_madnConstant;

        static const uint8_t g_u8Max;
        static const uint8_t g_u8Mid;

        static const uint16_t g_u16Max;
        static const uint16_t g_u16Mid;

        static const uint32_t g_u32Max;
        static const uint32_t g_u32Mid;
    };

    /* static */
    template <typename PixelT>
    double PixUtils::screenTransferFunc(PixelT pixel,
                                        double mBal,
                                        double sClip,
                                        double hClip,
                                        double sExp,
                                        double hExp)
    {
        double tmpPix = PixUtils::clippingFunc(pixel, sClip, hClip);

        tmpPix = PixUtils::midtonesTransferFunc(tmpPix, mBal);

        return PixUtils::expansionFunc(tmpPix, sExp, hExp);
    }

}