#pragma once

namespace ELS
{

    enum PixelFormat
    {
        PF_GRAY,
        PF_BGR,
        PF_RGB
    };

    enum SampleFormat
    {
        SF_INT_8,
        SF_INT_16,
        SF_INT_32,
        SF_UINT_8,
        SF_UINT_16,
        SF_UINT_32,
        SF_FLOAT,
        SF_DOUBLE
    };

    enum RasterFormat
    {
        RF_INTERLEAVED,
        RF_PLANAR
    };

}