#pragma once

#include <inttypes.h>
#include "fits.h"
#include "fitspixelvisitor.h"
namespace ELS
{

    class FITSImage
    {
    public:
        static FITSImage *load(const char *filename);

    public:
        ~FITSImage();

        const char *getImageType() const;
        const char *getSizeAndColor() const;

        int getWidth() const;
        int getHeight() const;
        FITS::RasterFormat getRasterFormat() const;
        bool isColor() const;

        FITS::BitDepth getBitDepth() const;

        const void *getPixels() const;

        template <typename PixelT>
        void visitPixels(FITSPixelVisitor<PixelT> *visitor) const;

    private:
        FITSImage(FITS::BitDepth bitDepth,
                  FITS::RasterFormat format,
                  bool isColor,
                  int width,
                  int height,
                  int64_t pixelCount,
                  void *pixels);

    private:
        static void *readPix(fitsfile *fits,
                             FITS::BitDepth bitDepth,
                             int64_t pixelCount);

    private:
        FITS::BitDepth _bitDepth;
        FITS::RasterFormat _format;
        bool _isColor;
        int _width;
        int _height;
        int64_t _pixelCount;
        void *_pixels;
    };

    template <typename PixelT>
    void FITSImage::visitPixels(FITSPixelVisitor<PixelT> *visitor) const
    {
        PixelT *ptPixels = (PixelT *)_pixels;
        PixelT vals[3] = {0, 0, 0};

        if (!_isColor)
        {
            visitor->pixelFormat(ELS::FITS::PF_GRAY);
            visitor->dimensions(_width, _height);
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    vals[0] = ptPixels[y * _width + x];
                    visitor->pixel(x, y, vals);
                }
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::FITS::PF_RGB);
            visitor->dimensions(_width, _height);
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    switch (_format)
                    {
                    case ELS::FITS::RF_INTERLEAVED:
                        vals[0] = ptPixels[3 * (x + _width * y) + 2];
                        vals[1] = ptPixels[3 * (x + _width * y) + 1];
                        vals[2] = ptPixels[3 * (x + _width * y) + 0];
                        break;
                    case ELS::FITS::RF_STRIDED:
                        vals[0] = ptPixels[x + _width * y + 2 * _width * _height];
                        vals[1] = ptPixels[x + _width * y + 1 * _width * _height];
                        vals[2] = ptPixels[x + _width * y + 0 * _width * _height];
                        break;
                    }

                    visitor->pixel(x, y, vals);
                }
            }
            visitor->done();
        }
    }

}
