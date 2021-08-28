#pragma once

#include "pixelvisitor.h"
#include "rastertypes.h"

namespace ELS
{

    class Image
    {
    public:
        virtual ~Image();

        virtual bool isColor() const = 0;

        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;

        virtual RasterFormat getRasterFormat() const = 0;
        virtual SampleFormat getSampleFormat() const = 0;

        virtual void visitPixels(PixelVisitor* visitor) const = 0;

        const char* getImageType() const;
        const char* getSizeAndColor() const;
    };

}