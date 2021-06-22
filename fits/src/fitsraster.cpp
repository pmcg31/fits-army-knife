#include "fitstantrum.h"
#include "fitsraster.h"

namespace ELS
{

    FITSRaster::FITSRaster(FITS::BitDepth bitDepth,
                           int64_t pixelCount)
        : _bitDepth(bitDepth),
          _pixelCount(pixelCount),
          _minPixelVal(),
          _maxPixelVal(),
          _pixels(0)
    {
        _minPixelVal.bitDepth = _bitDepth;
        _maxPixelVal.bitDepth = _bitDepth;

        switch (_bitDepth)
        {
        case FITS::BD_INT_8:
            _minPixelVal.i8 = ~0;
            _maxPixelVal.i8 = 0;
            break;
        case FITS::BD_INT_16:
            _minPixelVal.i16 = ~0;
            _maxPixelVal.i16 = 0;
            break;
        case FITS::BD_INT_32:
            _minPixelVal.i32 = ~0;
            _maxPixelVal.i32 = 0;
            break;
        case FITS::BD_FLOAT:
            _minPixelVal.f32 = 1.0;
            _maxPixelVal.f32 = 0.0;
            break;
        case FITS::BD_DOUBLE:
            _minPixelVal.f64 = 1.0;
            _maxPixelVal.f64 = 0.0;
            break;
        }
    }

    FITSRaster::~FITSRaster()
    {
        if (_pixels != 0)
        {
            switch (_bitDepth)
            {
            case FITS::BD_INT_8:
                delete[](uint8_t *) _pixels;
                break;
            case FITS::BD_INT_16:
                delete[](uint16_t *) _pixels;
                break;
            case FITS::BD_INT_32:
                delete[](uint32_t *) _pixels;
                break;
            case FITS::BD_FLOAT:
                delete[](float *) _pixels;
                break;
            case FITS::BD_DOUBLE:
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
        case FITS::BD_INT_8:
            fitsIOType = TBYTE;
            _pixels = new uint8_t[_pixelCount];
            break;
        case FITS::BD_INT_16:
            fitsIOType = TUSHORT;
            _pixels = new uint16_t[_pixelCount];
            break;
        case FITS::BD_INT_32:
            fitsIOType = TUINT;
            _pixels = new uint32_t[_pixelCount];
            break;
        case FITS::BD_FLOAT:
            fitsIOType = TFLOAT;
            _pixels = new float[_pixelCount];
            break;
        case FITS::BD_DOUBLE:
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

        switch (_bitDepth)
        {
        case FITS::BD_INT_8:
        {
            uint8_t *i8Pixels = (uint8_t *)_pixels;
            for (int64_t i = 0; i < _pixelCount; i++)
            {
                if (i8Pixels[i] > _maxPixelVal.i8)
                {
                    _maxPixelVal.i8 = i8Pixels[i];
                }
                if (i8Pixels[i] < _minPixelVal.i8)
                {
                    _minPixelVal.i8 = i8Pixels[i];
                }
            }
        }
        break;
        case FITS::BD_INT_16:
        {
            uint16_t *i16Pixels = (uint16_t *)_pixels;
            for (int64_t i = 0; i < _pixelCount; i++)
            {
                if (i16Pixels[i] > _maxPixelVal.i16)
                {
                    _maxPixelVal.i16 = i16Pixels[i];
                }
                if (i16Pixels[i] < _minPixelVal.i16)
                {
                    _minPixelVal.i16 = i16Pixels[i];
                }
            }
        }
        break;
        case FITS::BD_INT_32:
        {
            uint32_t *i32Pixels = (uint32_t *)_pixels;
            for (int64_t i = 0; i < _pixelCount; i++)
            {
                if (i32Pixels[i] > _maxPixelVal.i32)
                {
                    _maxPixelVal.i32 = i32Pixels[i];
                }
                if (i32Pixels[i] < _minPixelVal.i32)
                {
                    _minPixelVal.i32 = i32Pixels[i];
                }
            }
        }
        break;
        case FITS::BD_FLOAT:
        {
            float *f32Pixels = (float *)_pixels;
            for (int64_t i = 0; i < _pixelCount; i++)
            {
                if (f32Pixels[i] > _maxPixelVal.f32)
                {
                    _maxPixelVal.f32 = f32Pixels[i];
                }
                if (f32Pixels[i] < _minPixelVal.f32)
                {
                    _minPixelVal.f32 = f32Pixels[i];
                }
            }
        }
        break;
        case FITS::BD_DOUBLE:
        {
            double *f64Pixels = (double *)_pixels;
            for (int64_t i = 0; i < _pixelCount; i++)
            {
                if (f64Pixels[i] > _maxPixelVal.f64)
                {
                    _maxPixelVal.f64 = f64Pixels[i];
                }
                if (f64Pixels[i] < _minPixelVal.f64)
                {
                    _minPixelVal.f64 = f64Pixels[i];
                }
            }
        }
        break;
        }
    }

    const FITSVariant FITSRaster::getMinPixelVal() const
    {
        return _minPixelVal;
    }

    const FITSVariant FITSRaster::getMaxPixelVal() const
    {
        return _maxPixelVal;
    }

    const void *FITSRaster::getPixels() const
    {
        return _pixels;
    }

}
