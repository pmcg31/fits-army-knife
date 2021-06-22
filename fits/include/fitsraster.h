#pragma once

#include <inttypes.h>
#include <fitsio.h>

#include "fits.h"
#include "fitsimage.h"
#include "fitsvariant.h"

namespace ELS
{

    class FITSRaster
    {
    public:
        FITSRaster(FITS::BitDepth bitDepth,
                   int64_t pixelCount);
        ~FITSRaster();

        void readPix(fitsfile *fits);

        const FITSVariant getMinPixelVal() const;
        const FITSVariant getMaxPixelVal() const;

        const void *getPixels() const;

    private:
        FITS::BitDepth _bitDepth;
        int64_t _pixelCount;
        FITSVariant _minPixelVal;
        FITSVariant _maxPixelVal;
        void *_pixels;
    };

}
