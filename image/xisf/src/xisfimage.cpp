#include "rastertypes.h"
#include "xisfimage.h"
#include "xisfexception.h"

namespace ELS
{

    /* static  */
    XISFImage* XISFImage::load(const char* filename)
    {
        char errTxt[1024];

        pcl::XISFReader reader;

        reader.Open(filename);

        pcl::ImageOptions options = reader.ImageOptions();
        pcl::ImageInfo info = reader.ImageInfo();

        if (options.complexSample)
        {
            sprintf(errTxt,
                    "Image '%s' has complex samples; only real samples are supported at this time",
                    filename);
            throw new XISFException(errTxt);
        }

        ELS::SampleFormat sampleFormat = ELS::SF_UINT_16;
        bool sampleFormatDecoded = false;
        if (options.ieeefpSampleFormat)
        {
            if (options.bitsPerSample == 32)
            {
                sampleFormat = ELS::SF_FLOAT;
                sampleFormatDecoded = true;
            }
            else
            {
                sampleFormat = ELS::SF_DOUBLE;
                sampleFormatDecoded = true;
            }
        }
        else
        {
            if (options.signedIntegers)
            {
                if (options.bitsPerSample == 32)
                {
                    sampleFormat = ELS::SF_INT_32;
                    sampleFormatDecoded = true;
                }
                else if (options.bitsPerSample == 16)
                {
                    sampleFormat = ELS::SF_INT_16;
                    sampleFormatDecoded = true;
                }
                else if (options.bitsPerSample == 8)
                {
                    sampleFormat = ELS::SF_INT_8;
                    sampleFormatDecoded = true;
                }
            }
            else
            {
                if (options.bitsPerSample == 32)
                {
                    sampleFormat = ELS::SF_UINT_32;
                    sampleFormatDecoded = true;
                }
                else if (options.bitsPerSample == 16)
                {
                    sampleFormat = ELS::SF_UINT_16;
                    sampleFormatDecoded = true;
                }
                else if (options.bitsPerSample == 8)
                {
                    sampleFormat = ELS::SF_UINT_8;
                    sampleFormatDecoded = true;
                }
            }
        }
        if (!sampleFormatDecoded)
        {
            throw new XISFException("Unknown sample format");
        }

        bool isColor = false;
        switch (info.colorSpace)
        {
        case pcl::ColorSpace::Gray:
            isColor = false;
            break;
        case pcl::ColorSpace::RGB:
            isColor = true;
            break;
        case pcl::ColorSpace::CIEXYZ:
            sprintf(errTxt,
                    "Image '%s' has color space CIEXYZ; only RGB color space is supported at this time",
                    filename);
            throw new XISFException(errTxt);
            break;
        case pcl::ColorSpace::CIELab:
            sprintf(errTxt,
                    "Image '%s' has color space CIELab; only RGB color space is supported at this time",
                    filename);
            throw new XISFException(errTxt);
            break;
        case pcl::ColorSpace::CIELch:
            sprintf(errTxt,
                    "Image '%s' has color space CIELch; only RGB color space is supported at this time",
                    filename);
            throw new XISFException(errTxt);
            break;
        case pcl::ColorSpace::HSV:
            sprintf(errTxt,
                    "Image '%s' has color space HSV; only RGB color space is supported at this time",
                    filename);
            throw new XISFException(errTxt);
            break;
        case pcl::ColorSpace::HSI:
            sprintf(errTxt,
                    "Image '%s' has color space HSI; only RGB color space is supported at this time",
                    filename);
            throw new XISFException(errTxt);
            break;
        default:
            sprintf(errTxt,
                    "Image '%s' has unknown color space",
                    filename);
            throw new XISFException(errTxt);
            break;
        }

        printf("Channels: %d\n", info.numberOfChannels);
        printf("Size: %dx%d\n", info.width, info.height);

        XISFImage* tmp = 0;
        switch (sampleFormat)
        {
        case ELS::SF_INT_8:
        case ELS::SF_UINT_8:
        {
            pcl::UInt8Image* img = new pcl::UInt8Image();
            reader.ReadImage(*img);
            tmp = new XISFImage(sampleFormat,
                                isColor,
                                info.width,
                                info.height,
                                img);
        }
        break;
        case ELS::SF_INT_16:
        case ELS::SF_UINT_16:
        {
            pcl::UInt16Image* img = new pcl::UInt16Image();
            reader.ReadImage(*img);
            tmp = new XISFImage(sampleFormat,
                                isColor,
                                info.width,
                                info.height,
                                img);
        }
        break;
        case ELS::SF_INT_32:
        case ELS::SF_UINT_32:
        {
            pcl::UInt32Image* img = new pcl::UInt32Image();
            reader.ReadImage(*img);
            tmp = new XISFImage(sampleFormat,
                                isColor,
                                info.width,
                                info.height,
                                img);
        }
        break;
        case ELS::SF_FLOAT:
        {
            pcl::FImage* img = new pcl::FImage();
            reader.ReadImage(*img);
            tmp = new XISFImage(sampleFormat,
                                isColor,
                                info.width,
                                info.height,
                                img);
        }
        break;
        case ELS::SF_DOUBLE:
        {
            pcl::DImage* img = new pcl::DImage();
            reader.ReadImage(*img);
            tmp = new XISFImage(sampleFormat,
                                isColor,
                                info.width,
                                info.height,
                                img);
        }
        break;
        }

        reader.Close();

        return tmp;
    }

    XISFImage::XISFImage(SampleFormat sampleFormat,
                         bool isColor,
                         int width,
                         int height,
                         pcl::UInt8Image* img)
        : _sampleFormat(sampleFormat),
          _isColor(isColor),
          _width(width),
          _height(height),
          _pixels({.u8 = img})
    {
    }

    XISFImage::XISFImage(SampleFormat sampleFormat,
                         bool isColor,
                         int width,
                         int height,
                         pcl::UInt16Image* img)
        : _sampleFormat(sampleFormat),
          _isColor(isColor),
          _width(width),
          _height(height),
          _pixels({.u16 = img})
    {
    }

    XISFImage::XISFImage(SampleFormat sampleFormat,
                         bool isColor,
                         int width,
                         int height,
                         pcl::UInt32Image* img)
        : _sampleFormat(sampleFormat),
          _isColor(isColor),
          _width(width),
          _height(height),
          _pixels({.u32 = img})
    {
    }

    XISFImage::XISFImage(SampleFormat sampleFormat,
                         bool isColor,
                         int width,
                         int height,
                         pcl::FImage* img)
        : _sampleFormat(sampleFormat),
          _isColor(isColor),
          _width(width),
          _height(height),
          _pixels({.f = img})
    {
    }

    XISFImage::XISFImage(SampleFormat sampleFormat,
                         bool isColor,
                         int width,
                         int height,
                         pcl::DImage* img)
        : _sampleFormat(sampleFormat),
          _isColor(isColor),
          _width(width),
          _height(height),
          _pixels({.d = img})
    {
    }

    XISFImage::~XISFImage()
    {
        switch (_sampleFormat)
        {
        case ELS::SF_INT_8:
        case ELS::SF_UINT_8:
            if (_pixels.u8 != 0)
            {
                delete _pixels.u8;
                _pixels.u8 = 0;
            }
            break;
        case ELS::SF_INT_16:
        case ELS::SF_UINT_16:
            if (_pixels.u16 != 0)
            {
                delete _pixels.u16;
                _pixels.u16 = 0;
            }
            break;
        case ELS::SF_INT_32:
        case ELS::SF_UINT_32:
            if (_pixels.u32 != 0)
            {
                delete _pixels.u32;
                _pixels.u32 = 0;
            }
            break;
        case ELS::SF_FLOAT:
            if (_pixels.f != 0)
            {
                delete _pixels.f;
                _pixels.f = 0;
            }
            break;
        case ELS::SF_DOUBLE:
            if (_pixels.d != 0)
            {
                delete _pixels.d;
                _pixels.d = 0;
            }
            break;
        }
    }

    bool XISFImage::isColor() const
    {
        return _isColor;
    }

    int XISFImage::getWidth() const
    {
        return _width;
    }

    int XISFImage::getHeight() const
    {
        return _height;
    }

    RasterFormat XISFImage::getRasterFormat() const
    {
        return ELS::RF_PLANAR;
    }

    SampleFormat XISFImage::getSampleFormat() const
    {
        return _sampleFormat;
    }

    void XISFImage::visitPixels(PixelVisitor* visitor) const
    {
        switch (_sampleFormat)
        {
        case SF_INT_8:
        case SF_UINT_8:
            visitPixels(_pixels.u8,
                        visitor);
            break;
        case SF_INT_16:
        case SF_UINT_16:
            visitPixels(_pixels.u16,
                        visitor);
            break;
        case SF_INT_32:
        case SF_UINT_32:
            visitPixels(_pixels.u32,
                        visitor);
            break;
        case SF_FLOAT:
            visitPixels(_pixels.f,
                        visitor);
            break;
        case SF_DOUBLE:
            visitPixels(_pixels.d,
                        visitor);
            break;
        }
    }

    void XISFImage::visitPixels(pcl::UInt8Image* img,
                                PixelVisitor* visitor) const
    {
        if (!_isColor)
        {
            visitor->pixelFormat(ELS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                uint8_t* k = img->ScanLine(y, 0);
                visitor->rowGray(y, k);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::PF_RGB);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                uint8_t* rgb[3];
                for (int chan = 0; chan < 3; chan++)
                {
                    rgb[chan] = img->ScanLine(y, chan);
                }

                visitor->rowRgb(y,
                                rgb[0],
                                rgb[1],
                                rgb[2]);
            }
            visitor->done();
        }
    }

    void XISFImage::visitPixels(pcl::UInt16Image* img,
                                PixelVisitor* visitor) const
    {
        if (!_isColor)
        {
            visitor->pixelFormat(ELS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                uint16_t* k = img->ScanLine(y, 0);
                visitor->rowGray(y, k);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::PF_RGB);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                uint16_t* rgb[3];
                for (int chan = 0; chan < 3; chan++)
                {
                    rgb[chan] = img->ScanLine(y, chan);
                }

                visitor->rowRgb(y,
                                rgb[0],
                                rgb[1],
                                rgb[2]);
            }
            visitor->done();
        }
    }
    void XISFImage::visitPixels(pcl::UInt32Image* img,
                                PixelVisitor* visitor) const
    {
        if (!_isColor)
        {
            visitor->pixelFormat(ELS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                uint32_t* k = img->ScanLine(y, 0);
                visitor->rowGray(y, k);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::PF_RGB);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                uint32_t* rgb[3];
                for (int chan = 0; chan < 3; chan++)
                {
                    rgb[chan] = img->ScanLine(y, chan);
                }

                visitor->rowRgb(y,
                                rgb[0],
                                rgb[1],
                                rgb[2]);
            }
            visitor->done();
        }
    }
    void XISFImage::visitPixels(pcl::FImage* img,
                                PixelVisitor* visitor) const
    {
        if (!_isColor)
        {
            visitor->pixelFormat(ELS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                float* k = img->ScanLine(y, 0);
                visitor->rowGray(y, k);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::PF_RGB);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                float* rgb[3];
                for (int chan = 0; chan < 3; chan++)
                {
                    rgb[chan] = img->ScanLine(y, chan);
                }

                visitor->rowRgb(y,
                                rgb[0],
                                rgb[1],
                                rgb[2]);
            }
            visitor->done();
        }
    }
    void XISFImage::visitPixels(pcl::DImage* img,
                                PixelVisitor* visitor) const
    {
        if (!_isColor)
        {
            visitor->pixelFormat(ELS::PF_GRAY);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                double* k = img->ScanLine(y, 0);
                visitor->rowGray(y, k);
            }
            visitor->done();
        }
        else
        {
            visitor->pixelFormat(ELS::PF_RGB);
            visitor->dimensions(_width, _height);
            visitor->rowInfo(1);
            for (int y = 0; y < _height; y++)
            {
                double* rgb[3];
                for (int chan = 0; chan < 3; chan++)
                {
                    rgb[chan] = img->ScanLine(y, chan);
                }

                visitor->rowRgb(y,
                                rgb[0],
                                rgb[1],
                                rgb[2]);
            }
            visitor->done();
        }
    }

}