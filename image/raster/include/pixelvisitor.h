#pragma once

#include "pixelformat.h"

namespace ELS
{

    template <typename PixelT>
    class PixelVisitor
    {
    public:
        virtual void pixelFormat(ELS::PixelFormat pf) = 0;
        virtual void dimensions(int width, int height) = 0;
        virtual void rowInfo(int stride) = 0;
        virtual void rowGray(int y,
                             const PixelT *k) = 0;
        virtual void rowRgb(int y,
                            const PixelT *r,
                            const PixelT *g,
                            const PixelT *b) = 0;
        virtual void done() = 0;
    };

}