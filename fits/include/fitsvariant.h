#pragma once

#include <inttypes.h>

#include "fits.h"

namespace ELS
{
    struct FITSVariant
    {
        FITS::BitDepth bitDepth;

        union
        {
            uint8_t i8;
            uint16_t i16;
            uint32_t i32;
            float f32;
            double f64;
        };
    };

}