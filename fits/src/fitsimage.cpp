#include <fitsio.h>

#include "fitstantrum.h"
#include "fitsraster.h"
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

        /* Get the axis count for the image */
        int numAxis;
        fits_get_img_dim(tmpFits, &numAxis, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        /* Find the x/y-axis dimensions and the color dimension if it exists. */
        if (numAxis < 2)
        {
            throw new FITSException("Too few axes to be a real image!");
        }
        else if (numAxis > 3)
        {
            throw new FITSException("Too many axes to be a real image!");
        }

        /* Get the size of each axis */
        long axLengths[3];
        fits_get_img_size(tmpFits, 3, axLengths, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        /* Find the color axis if it exists.. */
        bool isColor;
        FITS::PixelFormat format;
        int width;
        int height;
        if (numAxis == 2)
        {
            isColor = false;
            format = FITS::PF_STRIDED;
            width = axLengths[1 - 1];
            height = axLengths[2 - 1];
        }
        else
        {
            isColor = true;
            if (axLengths[3 - 1] == 3)
            {
                format = FITS::PF_STRIDED;
                width = axLengths[1 - 1];
                height = axLengths[2 - 1];
            }
            else if (axLengths[1 - 1] == 3)
            {
                format = FITS::PF_INTERLEAVED;
                width = axLengths[2 - 1];
                height = axLengths[3 - 1];
            }
            else
            {
                throw new FITSException("Found 3 axis, but can't figure out RGB dimension!");
            }
        }

        /* Compute the number of pixels */
        int numPixels = width * height;
        if (isColor)
        {
            numPixels *= 3;
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

        // Create a raster for the data and read it
        FITSRaster *raster = new FITSRaster(bitDepth, numPixels);
        raster->readPix(tmpFits);

        //    /* Compute maximum pixel value */
        //    tmpInfo->maxPixelVal =  tmpInfo->imageArray[0];
        //    tmpInfo->minPixelVal =  tmpInfo->imageArray[0];
        //    for (int i = 1; i < tmpInfo->numPixels; i++) {
        //        if (tmpInfo->imageArray[i] > tmpInfo->maxPixelVal) {
        //            tmpInfo->maxPixelVal = tmpInfo->imageArray[i];
        //        }

        //        if (tmpInfo->imageArray[i] < tmpInfo->minPixelVal) {
        //            tmpInfo->minPixelVal = tmpInfo->imageArray[i];
        //        }
        //    }

        return new FITSImage(bitDepth,
                             format,
                             isColor,
                             width,
                             height,
                             raster);
    }

    FITSImage::FITSImage(FITS::BitDepth bitDepth,
                         FITS::PixelFormat format,
                         bool isColor,
                         int width,
                         int height,
                         FITSRaster *raster)
        : _bitDepth(bitDepth),
          _format(format),
          _isColor(isColor),
          _width(width),
          _height(height),
          _raster(raster)
    {
    }

    FITSImage::~FITSImage()
    {
        if (_raster != 0)
        {
            delete _raster;
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

    FITS::PixelFormat FITSImage::getPixelFormat() const
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

    const FITSVariant FITSImage::getMinPixelVal() const
    {
        return _raster->getMinPixelVal();
    }

    const FITSVariant FITSImage::getMaxPixelVal() const
    {
        return _raster->getMaxPixelVal();
    }

    const void *FITSImage::getPixels() const
    {
        return _raster->getPixels();
    }

}
