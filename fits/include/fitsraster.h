#pragma once

#include <inttypes.h>
#include <fitsio.h>

#include "fitsimage.h"

namespace ELS
{

    class FITSRaster
    {
    public:
        FITSRaster(FITSImage::BitDepth bitDepth,
                   int64_t pixelCount);
        ~FITSRaster();

        void readPix(fitsfile *fits);

        const void *getPixels() const;

    private:
        FITSImage::BitDepth _bitDepth;
        int64_t _pixelCount;
        void *_pixels;
    };

}
