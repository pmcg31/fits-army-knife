#pragma once

// Need to define this properly, but it's all linux all the time right now
#define __PCL_LINUX

// Suppress the MANY warnings from PCL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wdangling-else"
#include <pcl/XISF.h>
#pragma GCC diagnostic pop

#include <inttypes.h>

#include "image.h"
#include "pixelvisitor.h"
#include "rastertypes.h"

namespace ELS
{

    class XISFImage : public Image
    {
    public:
        static XISFImage* load(const char* filename);

    public:
        virtual ~XISFImage() override;

        virtual bool isColor() const override;

        virtual int getWidth() const override;
        virtual int getHeight() const override;

        virtual RasterFormat getRasterFormat() const override;
        virtual SampleFormat getSampleFormat() const override;

        virtual void visitPixels(PixelVisitor* visitor) const override;

    private:
        XISFImage(SampleFormat sampleFormat,
                  bool isColor,
                  int width,
                  int height,
                  pcl::UInt8Image* img);
        XISFImage(SampleFormat sampleFormat,
                  bool isColor,
                  int width,
                  int height,
                  pcl::UInt16Image* img);
        XISFImage(SampleFormat sampleFormat,
                  bool isColor,
                  int width,
                  int height,
                  pcl::UInt32Image* img);
        XISFImage(SampleFormat sampleFormat,
                  bool isColor,
                  int width,
                  int height,
                  pcl::FImage* img);
        XISFImage(SampleFormat sampleFormat,
                  bool isColor,
                  int width,
                  int height,
                  pcl::DImage* img);

        void visitPixels(pcl::UInt8Image* img,
                         PixelVisitor* visitor) const;
        void visitPixels(pcl::UInt16Image* img,
                         PixelVisitor* visitor) const;
        void visitPixels(pcl::UInt32Image* img,
                         PixelVisitor* visitor) const;
        void visitPixels(pcl::FImage* img,
                         PixelVisitor* visitor) const;
        void visitPixels(pcl::DImage* img,
                         PixelVisitor* visitor) const;

    private:
        SampleFormat _sampleFormat;
        bool _isColor;
        int _width;
        int _height;
        union
        {
            pcl::UInt8Image* u8;
            pcl::UInt16Image* u16;
            pcl::UInt32Image* u32;
            pcl::FImage* f;
            pcl::DImage* d;
        } _pixels;
    };

}
