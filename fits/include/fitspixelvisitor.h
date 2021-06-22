#pragma once

#include "fits.h"

namespace ELS
{

    template <typename PixelT>
    class FITSPixelVisitor
    {
    public:
        virtual void pixelFormat(FITS::PixelFormat pf) = 0;
        virtual void dimensions(int width, int height) = 0;
        virtual void pixel(int x, int y, const PixelT *pixelVals) = 0;
        virtual void done() = 0;
    };

}