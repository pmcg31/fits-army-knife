#pragma once

namespace ELS
{

    namespace FITS
    {

        enum BitDepth
        {
            BD_INT_8,
            BD_INT_16,
            BD_INT_32,
            BD_FLOAT,
            BD_DOUBLE
        };

        enum RasterFormat
        {
            RF_INTERLEAVED,
            RF_PLANAR
        };

    }

}