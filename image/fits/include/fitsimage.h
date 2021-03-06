#pragma once

#include <fitsio.h>
#include <inttypes.h>

#include "image.h"
#include "pixelvisitor.h"
#include "rastertypes.h"

namespace ELS
{

    class FITSImage : public Image
    {
    public:
        static FITSImage* load(const char* filename);

    public:
        virtual ~FITSImage() override;

        virtual bool isColor() const override;

        virtual int getWidth() const override;
        virtual int getHeight() const override;

        virtual RasterFormat getRasterFormat() const override;
        virtual SampleFormat getSampleFormat() const override;

        virtual void visitPixels(PixelVisitor* visitor) const override;

    private:
        FITSImage(SampleFormat sampleFormat,
                  RasterFormat format,
                  bool isColor,
                  int width,
                  int height,
                  void* pixels);

        template <typename PixelT>
        void visitPixels(PixelT* pixels,
                         PixelVisitor* visitor) const;

    private:
        static void* readPix(fitsfile* fits,
                             SampleFormat sampleFormat,
                             int64_t pixelCount);

    private:
        SampleFormat _sampleFormat;
        RasterFormat _format;
        bool _isColor;
        int _width;
        int _height;
        void* _pixels;
    };

}
