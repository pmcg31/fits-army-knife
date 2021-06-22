#pragma once

#include <inttypes.h>
#include "fits.h"
#include "fitsvariant.h"

namespace ELS
{

    class FITSRaster;

    class FITSImage
    {
    public:
        static FITSImage *load(const char *filename);

    public:
        ~FITSImage();

        const char *getImageType() const;
        const char *getSizeAndColor() const;

        int getWidth() const;
        int getHeight() const;
        FITS::PixelFormat getPixelFormat() const;
        bool isColor() const;

        FITS::BitDepth getBitDepth() const;
        const FITSVariant getMinPixelVal() const;
        const FITSVariant getMaxPixelVal() const;
        const void *getPixels() const;

    private:
        FITSImage(FITS::BitDepth bitDepth,
                  FITS::PixelFormat format,
                  bool isColor,
                  int width,
                  int height,
                  FITSRaster *raster);

    private:
        FITS::BitDepth _bitDepth;
        FITS::PixelFormat _format;
        bool _isColor;
        int _width;
        int _height;
        FITSRaster *_raster;
    };

}
