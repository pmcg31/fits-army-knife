#include "fitstantrum.h"
#include "fitsraster.h"

namespace ELS
{

    FITSRaster::FITSRaster(FITSImage::BitDepth bitDepth,
                           int64_t pixelCount)
        : _bitDepth(bitDepth),
          _pixelCount(pixelCount),
          _pixels(0)
    {
    }

    FITSRaster::~FITSRaster()
    {
        if (_pixels != 0)
        {
            switch (_bitDepth)
            {
            case FITSImage::BD_INT_8:
                delete[](uint8_t *) _pixels;
                break;
            case FITSImage::BD_INT_16:
                delete[](uint16_t *) _pixels;
                break;
            case FITSImage::BD_INT_32:
                delete[](uint32_t *) _pixels;
                break;
            case FITSImage::BD_FLOAT:
                delete[](float *) _pixels;
                break;
            case FITSImage::BD_DOUBLE:
                delete[](double *) _pixels;
                break;
            default:
                break;
            }
        }
    }

    void FITSRaster::readPix(fitsfile *fits)
    {
        long fpixel[3] = {1, 1, 1};

        // Allocate space for the pixels
        int fitsIOType = 0;
        switch (_bitDepth)
        {
        case FITSImage::BD_INT_8:
            fitsIOType = TBYTE;
            _pixels = new uint8_t[_pixelCount];
            break;
        case FITSImage::BD_INT_16:
            fitsIOType = TUSHORT;
            _pixels = new uint16_t[_pixelCount];
            break;
        case FITSImage::BD_INT_32:
            fitsIOType = TUINT;
            _pixels = new uint32_t[_pixelCount];
            break;
        case FITSImage::BD_FLOAT:
            fitsIOType = TFLOAT;
            _pixels = new float[_pixelCount];
            break;
        case FITSImage::BD_DOUBLE:
            fitsIOType = TDOUBLE;
            _pixels = new double[_pixelCount];
            break;
        default:
            throw new FITSException("Unknown bit depth");
        }

        // Read in the data in one big gulp
        int status = 0;
        fits_read_pix(fits,
                      fitsIOType,
                      fpixel,
                      _pixelCount,
                      NULL,
                      _pixels,
                      NULL,
                      &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }
    }

    const void *FITSRaster::getPixels() const
    {
        return _pixels;
    }

}
