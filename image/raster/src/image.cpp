#include <stdio.h>
#include "image.h"

namespace ELS
{
    Image::~Image() {}

    const char *Image::getImageType() const
    {
        SampleFormat sf = getSampleFormat();
        switch (sf)
        {
        case SF_INT_8:
            return "8-bit integer pixels";
        case SF_INT_16:
            return "16-bit integer pixels";
        case SF_INT_32:
            return "32-bit integer pixels";
        case SF_UINT_8:
            return "8-bit unsigned integer pixels";
        case SF_UINT_16:
            return "16-bit unsigned integer pixels";
        case SF_UINT_32:
            return "32-bit unsigned integer pixels";
        case SF_FLOAT:
            return "32-bit floating point pixels";
        case SF_DOUBLE:
            return "64-bit floating point pixels";
        }

        return "Unknown";
    }

    const char *Image::getSizeAndColor() const
    {
        static char tmp[50];

        sprintf(tmp, "%dx%d %s image",
                getWidth(),
                getHeight(),
                isColor() ? "Color" : "Grayscale");

        return tmp;
    }

}