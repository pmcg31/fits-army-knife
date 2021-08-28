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
        RasterFormat rasterFormat;
        int width;
        int height;
        if (numAxis == 2)
        {
            isColor = false;
            rasterFormat = RF_PLANAR;
            width = axLengths[0];
            height = axLengths[1];
        }
        else
        {
            isColor = true;
            if (axLengths[2] == 3)
            {
                rasterFormat = RF_PLANAR;
                width = axLengths[0];
                height = axLengths[1];
            }
            else if (axLengths[0] == 3)
            {
                rasterFormat = RF_INTERLEAVED;
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

        int fitsIOSampleFormat;
        fits_get_img_equivtype(tmpFits, &fitsIOSampleFormat, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        SampleFormat sampleFormat;
        switch (fitsIOSampleFormat)
        {
        case BYTE_IMG:
            sampleFormat = SF_UINT_8;
            break;
        case SBYTE_IMG:
            sampleFormat = SF_INT_8;
            break;
        case SHORT_IMG:
            sampleFormat = SF_INT_16;
            break;
        case USHORT_IMG:
            sampleFormat = SF_UINT_16;
            break;
        case LONG_IMG:
            sampleFormat = SF_INT_32;
            break;
        case ULONG_IMG:
            sampleFormat = SF_UINT_32;
            break;
        case FLOAT_IMG:
            sampleFormat = SF_FLOAT;
            break;
        case DOUBLE_IMG:
            sampleFormat = SF_DOUBLE;
            break;
        default:
            throw new FITSException("Unknown sample format");
        }

        void *pixels = readPix(tmpFits, sampleFormat, pixelCount);

        fits_close_file(tmpFits, &status);

        return new FITSImage(sampleFormat,
                             rasterFormat,
                             isColor,
                             width,
                             height,
                             pixelCount,
                             pixels);
    }

    FITSImage::FITSImage(SampleFormat sampleFormat,
                         RasterFormat format,
                         bool isColor,
                         int width,
                         int height,
                         int64_t pixelCount,
                         void *pixels)
        : _sampleFormat(sampleFormat),
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
            switch (_sampleFormat)
            {
            case SF_INT_8:
                delete[](int8_t *) _pixels;
                break;
            case SF_INT_16:
                delete[](int16_t *) _pixels;
                break;
            case SF_INT_32:
                delete[](int32_t *) _pixels;
                break;
            case SF_UINT_8:
                delete[](uint8_t *) _pixels;
                break;
            case SF_UINT_16:
                delete[](uint16_t *) _pixels;
                break;
            case SF_UINT_32:
                delete[](uint32_t *) _pixels;
                break;
            case SF_FLOAT:
                delete[](float *) _pixels;
                break;
            case SF_DOUBLE:
                delete[](double *) _pixels;
                break;
            default:
                break;
            }
        }
    }

    int FITSImage::getWidth() const
    {
        return _width;
    }

    int FITSImage::getHeight() const
    {
        return _height;
    }

    RasterFormat FITSImage::getRasterFormat() const
    {
        return _format;
    }

    bool FITSImage::isColor() const
    {
        return _isColor;
    }

    SampleFormat FITSImage::getSampleFormat() const
    {
        return _sampleFormat;
    }

    void FITSImage::visitPixels(PixelVisitor *visitor) const
    {
        switch (_sampleFormat)
        {
        case SF_INT_8:
            visitPixels((int8_t *)_pixels,
                        visitor);
            break;
        case SF_INT_16:
            visitPixels((int16_t *)_pixels,
                        visitor);
            break;
        case SF_INT_32:
            visitPixels((int32_t *)_pixels,
                        visitor);
            break;
        case SF_UINT_8:
            visitPixels((uint8_t *)_pixels,
                        visitor);
            break;
        case SF_UINT_16:
            visitPixels((uint16_t *)_pixels,
                        visitor);
            break;
        case SF_UINT_32:
            visitPixels((uint32_t *)_pixels,
                        visitor);
            break;
        case SF_FLOAT:
            visitPixels((float *)_pixels,
                        visitor);
            break;
        case SF_DOUBLE:
            visitPixels((double *)_pixels,
                        visitor);
            break;
        }
    }

    template <typename PixelT>
    void FITSImage::visitPixels(PixelT *pixels,
                                PixelVisitor *visitor) const
    {
        if (!_isColor)
        {
            visitor->pixelFormat(ELS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                visitor->rowGray(y, &pixels[y * _width]);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::PF_RGB);
            visitor->dimensions(_width, _height);
            switch (_format)
            {
            case RF_INTERLEAVED:
                visitor->rowInfo(3);
                break;
            case RF_PLANAR:
                visitor->rowInfo(1);
                break;
            }
            int gOffset = _width * _height;
            int bOffset = gOffset * 2;
            for (int y = 0; y < _height; y++)
            {
                int rowOffset = y * _width;
                switch (_format)
                {
                case RF_INTERLEAVED:
                    visitor->rowRgb(y,
                                    &pixels[3 * rowOffset + 0],
                                    &pixels[3 * rowOffset + 1],
                                    &pixels[3 * rowOffset + 2]);
                    break;
                case RF_PLANAR:
                    visitor->rowRgb(y,
                                    &pixels[rowOffset],
                                    &pixels[gOffset + rowOffset],
                                    &pixels[bOffset + rowOffset]);
                    break;
                }
            }
            visitor->done();
        }
    }

    /* static */
    void *FITSImage::readPix(fitsfile *fits,
                             SampleFormat sampleFormat,
                             int64_t pixelCount)
    {
        long fpixel[3] = {1, 1, 1};

        // Allocate space for the pixels
        int fitsIOType = 0;
        void *pixels;
        switch (sampleFormat)
        {
        case SF_INT_8:
            fitsIOType = TSBYTE;
            pixels = new int8_t[pixelCount];
            break;
        case SF_INT_16:
            fitsIOType = TSHORT;
            pixels = new int16_t[pixelCount];
            break;
        case SF_INT_32:
            fitsIOType = TINT;
            pixels = new int32_t[pixelCount];
            break;
        case SF_UINT_8:
            fitsIOType = TBYTE;
            pixels = new uint8_t[pixelCount];
            break;
        case SF_UINT_16:
            fitsIOType = TUSHORT;
            pixels = new uint16_t[pixelCount];
            break;
        case SF_UINT_32:
            fitsIOType = TUINT;
            pixels = new uint32_t[pixelCount];
            break;
        case SF_FLOAT:
            fitsIOType = TFLOAT;
            pixels = new float[pixelCount];
            break;
        case SF_DOUBLE:
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
