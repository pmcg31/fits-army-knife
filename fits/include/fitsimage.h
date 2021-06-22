#pragma once

#include <inttypes.h>

namespace ELS
{

    class FITSRaster;

    class FITSImage
    {
    public:
        enum BitDepth
        {
            BD_INT_8,
            BD_INT_16,
            BD_INT_32,
            BD_FLOAT,
            BD_DOUBLE
        };

    public:
        enum PixelFormat
        {
            PF_INTERLEAVED,
            PF_STRIDED
        };

    public:
        class Info
        {
        public:
            int bitDepthEnum;
            char imageType[100];
            int numAxis;
            long axLengths[3];
            int chanAx;
            int width;
            int height;
            int64_t numPixels;
            char sizeAndColor[200];
            long fpixel[3];
            double *imageArray;
            double maxPixelVal;
            double minPixelVal;
        };

    public:
        static FITSImage *load(const char *filename);

    public:
        ~FITSImage();

        const char *getImageType() const;
        const char *getSizeAndColor() const;

        int getWidth() const;
        int getHeight() const;
        PixelFormat getPixelFormat() const;
        bool isColor() const;

        BitDepth getBitDepth() const;
        const void *getPixels() const;

    private:
        FITSImage(BitDepth bitDepth,
                  PixelFormat format,
                  bool isColor,
                  int width,
                  int height,
                  FITSRaster *raster);

    private:
        BitDepth _bitDepth;
        PixelFormat _format;
        bool _isColor;
        int _width;
        int _height;
        FITSRaster *_raster;
    };

}
