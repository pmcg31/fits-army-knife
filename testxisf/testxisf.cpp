#include <QApplication>
#include <QImage>
#include <inttypes.h>

#define __PCL_LINUX

// Suppress the MANY warnings from PCL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wdangling-else"
#include <pcl/XISF.h>
#pragma GCC diagnostic pop

#include "mainwindow.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("You gotta give me a file!\n");
        return 1;
    }

    char *filename = argv[1];

    printf("File: %s\n", filename);

    pcl::XISFReader reader;

    reader.Open(filename);

    pcl::ImageOptions options = reader.ImageOptions();
    pcl::ImageInfo info = reader.ImageInfo();

    const char *sampleFormat = "Unknown";
    if (options.ieeefpSampleFormat)
    {
        if (options.complexSample)
        {
            sampleFormat = "Complex Floating Point";
        }
        else
        {
            sampleFormat = "Floating Point";
        }
    }
    else
    {
        if (options.signedIntegers)
        {
            if (options.complexSample)
            {
                sampleFormat = "Complex Signed Integer";
            }
            else
            {
                sampleFormat = "Signed Integer";
            }
        }
        else
        {
            if (options.complexSample)
            {
                sampleFormat = "Complex Unsigned Integer";
            }
            else
            {
                sampleFormat = "Unsigned Integer";
            }
        }
    }
    printf("Sample format: %d-bit %s\n", options.bitsPerSample, sampleFormat);

    const char *colorSpace = "Unknown";
    switch (info.colorSpace)
    {
    case pcl::ColorSpace::Gray:
        colorSpace = "Grayscale";
        break;
    case pcl::ColorSpace::RGB:
        colorSpace = "RGB";
        break;
    case pcl::ColorSpace::CIEXYZ:
        colorSpace = "CIEXYZ";
        break;
    case pcl::ColorSpace::CIELab:
        colorSpace = "CIELab";
        break;
    case pcl::ColorSpace::CIELch:
        colorSpace = "CIELch";
        break;
    case pcl::ColorSpace::HSV:
        colorSpace = "HSV";
        break;
    case pcl::ColorSpace::HSI:
        colorSpace = "HSI";
        break;
    default:
        colorSpace = "Unknown";
        break;
    }

    printf("Channels: %d\n", info.numberOfChannels);
    printf("Color space: %s\n", colorSpace);
    printf("Size: %dx%d\n", info.width, info.height);

    pcl::FImage effinImage;
    reader.ReadImage(effinImage);

    reader.Close();

    uint32_t pixCount = info.width * info.height;
    uint32_t *imgData = new uint32_t[pixCount];
    if (info.numberOfChannels == 1)
    {
        for (int y = 0, rowOffset = 0; y < info.height; y++, rowOffset += info.width)
        {
            float *k = effinImage.ScanLine(y, 0);

            for (int x = 0; x < info.width; x++)
            {
                uint8_t val = (uint8_t)(k[x] * 255);
                imgData[rowOffset + x] = (0xff << 24) |
                                         (val << 16) |
                                         (val << 8) |
                                         (val);
            }
        }
    }
    else
    {
        for (int y = 0, rowOffset = 0; y < info.height; y++, rowOffset += info.width)
        {

            float *rgb[3];
            for (int chan = 0; chan < info.numberOfChannels; chan++)
            {
                rgb[chan] = effinImage.ScanLine(y, chan);
            }

            for (int x = 0; x < info.width; x++)
            {
                uint8_t red = (uint8_t)(rgb[0][x] * 255);
                uint8_t green = (uint8_t)(rgb[1][x] * 255);
                uint8_t blue = (uint8_t)(rgb[2][x] * 255);
                imgData[rowOffset + x] = (0xff << 24) |
                                         (red << 16) |
                                         (green << 8) |
                                         (blue);
            }
        }
    }

    QImage qImage((const uchar *)imgData, info.width, info.height, QImage::Format_ARGB32);

    int noargc = 1;
    QApplication a(noargc, argv);
    MainWindow mw(qImage);

    mw.show();

    return a.exec();
}