#include <fitsio.h>

#include "fitstantrum.h"
#include "fitsimage.h"

namespace ELS
{

    /* static */
    FITSImage *FITSImage::load(const char *filename)
    {
        int status = 0;
        fitsfile *tmpFits;

        fits_open_file(&tmpFits, filename, READONLY, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        int numAxis;
        fits_get_img_dim(tmpFits, &numAxis, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        if ((numAxis < 2) || (numAxis > 3))
        {
            throw new FITSException("Unknown FITS format: unrecognized number of image axes");
        }

        long axLengths[3];
        fits_get_img_size(tmpFits, 3, axLengths, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        bool isColor;
        FITS::RasterFormat format;
        int width;
        int height;
        if (numAxis == 2)
        {
            isColor = false;
            format = FITS::RF_PLANAR;
            width = axLengths[0];
            height = axLengths[1];
        }
        else
        {
            isColor = true;
            if (axLengths[2] == 3)
            {
                format = FITS::RF_PLANAR;
                width = axLengths[0];
                height = axLengths[1];
            }
            else if (axLengths[0] == 3)
            {
                format = FITS::RF_INTERLEAVED;
                width = axLengths[1];
                height = axLengths[2];
            }
            else
            {
                throw new FITSException("Unknown FITS format: unrecognized axis layout");
            }
        }

        int64_t pixelCount = width * height;
        if (isColor)
        {
            pixelCount *= 3;
        }

        int fitsIOBitDepth;
        fits_get_img_equivtype(tmpFits, &fitsIOBitDepth, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        FITS::BitDepth bitDepth;
        switch (fitsIOBitDepth)
        {
        case BYTE_IMG:
        case SBYTE_IMG:
            bitDepth = FITS::BD_INT_8;
            break;
        case SHORT_IMG:
        case USHORT_IMG:
            bitDepth = FITS::BD_INT_16;
            break;
        case LONG_IMG:
        case ULONG_IMG:
            bitDepth = FITS::BD_INT_32;
            break;
        case FLOAT_IMG:
            bitDepth = FITS::BD_FLOAT;
            break;
        case DOUBLE_IMG:
            bitDepth = FITS::BD_DOUBLE;
            break;
        default:
            throw new FITSException("Unknown bit depth");
        }

        void *pixels = readPix(tmpFits, bitDepth, pixelCount);

        fits_close_file(tmpFits, &status);

        return new FITSImage(bitDepth,
                             format,
                             isColor,
                             width,
                             height,
                             pixelCount,
                             pixels);
    }

    FITSImage::FITSImage(FITS::BitDepth bitDepth,
                         FITS::RasterFormat format,
                         bool isColor,
                         int width,
                         int height,
                         int64_t pixelCount,
                         void *pixels)
        : _bitDepth(bitDepth),
          _format(format),
          _isColor(isColor),
          _width(width),
          _height(height),
          _pixelCount(pixelCount),
          _pixels(pixels)
    {
    }

    FITSImage::~FITSImage()
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

    const char *FITSImage::getImageType() const
    {
        switch (_bitDepth)
        {
        case FITS::BD_INT_8:
            return "8-bit integer pixels";
        case FITS::BD_INT_16:
            return "16-bit integer pixels";
        case FITS::BD_INT_32:
            return "32-bit integer pixels";
        case FITS::BD_FLOAT:
            return "32-bit floating point pixels";
        case FITS::BD_DOUBLE:
            return "64-bit floating point pixels";
        }

        return "Unknown";
    }

    const char *FITSImage::getSizeAndColor() const
    {
        static char tmp[50];

        sprintf(tmp, "%dx%d %s image", _width, _height, _isColor ? "Color" : "Grayscale");

        return tmp;
    }

    int FITSImage::getWidth() const
    {
        return _width;
    }

    int FITSImage::getHeight() const
    {
        return _height;
    }

    FITS::RasterFormat FITSImage::getRasterFormat() const
    {
        return _format;
    }

    bool FITSImage::isColor() const
    {
        return _isColor;
    }

    FITS::BitDepth FITSImage::getBitDepth() const
    {
        return _bitDepth;
    }

    const void *FITSImage::getPixels() const
    {
        return _pixels;
    }

    /* static */
    void *FITSImage::readPix(fitsfile *fits,
                             FITS::BitDepth bitDepth,
                             int64_t pixelCount)
    {
        long fpixel[3] = {1, 1, 1};

        // Allocate space for the pixels
        int fitsIOType = 0;
        void *pixels;
        switch (bitDepth)
        {
        case FITS::BD_INT_8:
            fitsIOType = TBYTE;
            pixels = new uint8_t[pixelCount];
            break;
        case FITS::BD_INT_16:
            fitsIOType = TUSHORT;
            pixels = new uint16_t[pixelCount];
            break;
        case FITS::BD_INT_32:
            fitsIOType = TUINT;
            pixels = new uint32_t[pixelCount];
            break;
        case FITS::BD_FLOAT:
            fitsIOType = TFLOAT;
            pixels = new float[pixelCount];
            break;
        case FITS::BD_DOUBLE:
            fitsIOType = TDOUBLE;
            pixels = new double[pixelCount];
            break;
        default:
            throw new FITSException("Unknown bit depth");
        }

        // Read in the data in one big gulp
        int status = 0;
        fits_read_pix(fits,
                      fitsIOType,
                      fpixel,
                      pixelCount,
                      NULL,
                      pixels,
                      NULL,
                      &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        return pixels;
    }

}
