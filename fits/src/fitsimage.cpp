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
        Info *tmpInfo = new Info();

        fits_open_file(&tmpFits, filename, READONLY, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        /* Get the axis count for the image */
        fits_get_img_dim(tmpFits, &tmpInfo->numAxis, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        /* Find the x/y-axis dimensions and the color dimension if it exists. */
        if (tmpInfo->numAxis < 2)
        {
            throw new FITSException("Too few axes to be a real image!");
        }
        else if (tmpInfo->numAxis > 3)
        {
            throw new FITSException("Too many axes to be a real image!");
        }

        /* Get the size of each axis */
        fits_get_img_size(tmpFits, 3, tmpInfo->axLengths, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        /* Find the color axis if it exists.. */
        if (tmpInfo->numAxis == 2)
        {
            tmpInfo->chanAx = 0;
            tmpInfo->width = tmpInfo->axLengths[1 - 1];
            tmpInfo->height = tmpInfo->axLengths[2 - 1];
        }
        else
        { // (numAxis == 3)
            if (tmpInfo->axLengths[3 - 1] == 3)
            {
                tmpInfo->chanAx = 3;
                tmpInfo->width = tmpInfo->axLengths[1 - 1];
                tmpInfo->height = tmpInfo->axLengths[2 - 1];
            }
            else if (tmpInfo->axLengths[1 - 1] == 3)
            {
                tmpInfo->chanAx = 1;
                tmpInfo->width = tmpInfo->axLengths[2 - 1];
                tmpInfo->height = tmpInfo->axLengths[3 - 1];
            }
            else
            {
                throw new FITSException("Found 3 axis, but can't figure out RGB dimension!");
            }
        }

        /* Compute the number of pixels */
        tmpInfo->numPixels = tmpInfo->width * tmpInfo->height;
        if (tmpInfo->chanAx != 0)
        {
            tmpInfo->numPixels *= 3;
        }

        /* Report on image size and color axis location */
        if (tmpInfo->chanAx)
        {
            sprintf(tmpInfo->sizeAndColor, "%dx%d Color FITS image; RGB is ax %d", tmpInfo->width, tmpInfo->height, tmpInfo->chanAx);
        }
        else
        {
            sprintf(tmpInfo->sizeAndColor, "%dx%d FITS image", tmpInfo->width, tmpInfo->height);
        }

        /* Set up fpixel for a full image read. */
        for (int i = 1; i <= tmpInfo->numAxis; i++)
        {
            tmpInfo->fpixel[i - 1] = 1;
        }

        int fitsIOBitDepth;
        fits_get_img_equivtype(tmpFits, &fitsIOBitDepth, &status);
        if (status)
        {
            throw new FITSTantrum(status);
        }

        FITSImage::BitDepth bitDepth;
        switch (fitsIOBitDepth)
        {
        case BYTE_IMG:
        case SBYTE_IMG:
            bitDepth = FITSImage::BD_INT_8;
            sprintf(tmpInfo->imageType, "8-bit byte pixels");
            break;
        case SHORT_IMG:
        case USHORT_IMG:
            bitDepth = FITSImage::BD_INT_16;
            sprintf(tmpInfo->imageType, "16-bit integer pixels");
            break;
        case LONG_IMG:
        case ULONG_IMG:
            bitDepth = FITSImage::BD_INT_32;
            sprintf(tmpInfo->imageType, "32-bit integer pixels");
            break;
        case FLOAT_IMG:
            bitDepth = FITSImage::BD_FLOAT;
            sprintf(tmpInfo->imageType, "32-bit floating point pixels");
            break;
        case DOUBLE_IMG:
            bitDepth = FITSImage::BD_DOUBLE;
            sprintf(tmpInfo->imageType, "64-bit floating point pixels");
            break;
        default:
            throw new FITSException("Unknown bit depth");
        }

        // Create a raster for the data and read it
        FITSRaster *raster = new FITSRaster(bitDepth, tmpInfo->numPixels);
        raster->readPix(tmpFits, tmpInfo->fpixel);

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

        return new FITSImage(bitDepth, raster, tmpInfo);
    }

    FITSImage::FITSImage(BitDepth bitDepth,
                         FITSRaster *raster,
                         Info *info)
        : _bitDepth(bitDepth), _raster(raster), _info(info)
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
        return _info->imageType;
    }

    const char *FITSImage::getSizeAndColor() const
    {
        return _info->sizeAndColor;
    }

    int FITSImage::getWidth() const
    {
        return _info->width;
    }

    int FITSImage::getHeight() const
    {
        return _info->height;
    }

    int FITSImage::getChanAx() const
    {
        return _info->chanAx;
    }

    bool FITSImage::isColor() const
    {
        return _info->chanAx != 0;
    }

    FITSImage::BitDepth FITSImage::getBitDepth() const
    {
        return _bitDepth;
    }

    const void *FITSImage::getPixels() const
    {
        return _raster->getPixels();
    }

}
