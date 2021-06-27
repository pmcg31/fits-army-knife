#pragma once

#include <inttypes.h>
#include <fitsio.h>
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

        if (!_isColor)
        {
            visitor->pixelFormat(ELS::FITS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                visitor->rowGray(y, &ptPixels[y * _width]);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::FITS::PF_RGB);
            visitor->dimensions(_width, _height);
            switch (_format)
            {
            case ELS::FITS::RF_INTERLEAVED:
                visitor->rowInfo(3);
                break;
            case ELS::FITS::RF_PLANAR:
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
                case ELS::FITS::RF_INTERLEAVED:
                    visitor->rowRgb(y,
                                    &ptPixels[3 * rowOffset + 0],
                                    &ptPixels[3 * rowOffset + 1],
                                    &ptPixels[3 * rowOffset + 2]);
                    break;
                case ELS::FITS::RF_PLANAR:
                    visitor->rowRgb(y,
                                    &ptPixels[rowOffset],
                                    &ptPixels[gOffset + rowOffset],
                                    &ptPixels[bOffset + rowOffset]);
                    break;
                }
            }
            visitor->done();
        }
    }

}
