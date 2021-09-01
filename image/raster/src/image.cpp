#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "imageloadexception.h"
#include "image.h"
#include "fitsimage.h"
#include "xisfimage.h"

namespace ELS
{

    const char* Image::g_fileTypeStr[] = {
        "Unknown",
        "FITS",
        "XISF",
    };

    Image::ExtInfo Image::g_extInfo[] = {
        {".fits", 5, FT_FITS},
        {".fit", 4, FT_FITS},
        {".fts", 4, FT_FITS},
        {".xisf", 5, FT_XISF},
        {0, 0, FT_UNKNOWN},
    };

    Image::MagicInfo Image::g_fitsMagic = {"SIMPLE", 6, FT_FITS};
    Image::MagicInfo Image::g_xisfMagic = {"XISF", 4, FT_XISF};

    const int Image::g_maxMagicLen = 6;

    /* static */
    Image* Image::load(const char* filename)
    {
        FileType fileType = fileTypeFromFilename(filename);
        printf("%s FileType: %s\n", filename, g_fileTypeStr[fileType]);

        return load(filename, fileType);
    }

    /* static */
    Image* Image::load(const char* filename,
                       FileType fileType)
    {
        switch (fileType)
        {
        case FT_FITS:
            return FITSImage::load(filename);
        case FT_XISF:
            return XISFImage::load(filename);
        case FT_UNKNOWN:
        default:
            throw new ImageLoadException("Could not determine image type from filename extension");
        }
    }

    /* static */
    Image::FileType Image::isSupportedFile(const char* filename,
                                           char* error /* = 0 */)
    {
        FileType fileType = fileTypeFromFilename(filename);

        switch (fileType)
        {
        case FT_FITS:
            if (checkMagic(filename, &g_fitsMagic, error))
            {
                return FT_FITS;
            }
            break;
        case FT_XISF:
            if (checkMagic(filename, &g_xisfMagic, error))
            {
                return FT_XISF;
            }
            break;
        case FT_UNKNOWN:
        default:
            if (error != 0)
            {
                sprintf(error, "%s: unknown file type", filename);
            }
            break;
        }

        return FT_UNKNOWN;
    }

    /* static */
    Image::FileType Image::fileTypeFromFilename(const char* filename)
    {
        size_t len = strlen(filename);

        for (int i = 0; g_extInfo[i].ext != 0; i++)
        {
            int fnIdx = len - g_extInfo[i].extLen;
            bool match = true;
            for (int exIdx = 0; exIdx < g_extInfo[i].extLen; exIdx++, fnIdx++)
            {
                if (tolower(filename[fnIdx]) != g_extInfo[i].ext[exIdx])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                return g_extInfo[i].fileType;
            }
        }

        return FT_UNKNOWN;
    }

    /* static */
    bool Image::checkMagic(const char* filename,
                           MagicInfo* magic,
                           char* error /* = 0 */)
    {
        char buf[g_maxMagicLen];

        FILE* fp = fopen(filename, "r");
        if (fp != NULL)
        {
            size_t readCount = fread(buf, 1, magic->magicLen, fp);
            fclose(fp);

            if (readCount == magic->magicLen)
            {
                for (size_t i = 0; i < magic->magicLen; i++)
                {
                    if (magic->magic[i] != buf[i])
                    {
                        if (error != 0)
                        {
                            sprintf(error, "File does not start with %s magic characters",
                                    g_fileTypeStr[magic->fileType]);
                        }
                        return false;
                    }
                }

                return true;
            }
            else
            {
                if (error != 0)
                {
                    sprintf(error, "File does not start with %s magic characters",
                            g_fileTypeStr[magic->fileType]);
                }
            }
        }
        else
        {
            if (error != 0)
            {
                sprintf(error, "File is not readable");
            }
        }

        return false;
    }

    Image::~Image() {}

    const char* Image::getImageType() const
    {
        SampleFormat sf = getSampleFormat();
        switch (sf)
        {
        case SF_INT_8:
            return "8-bit integer pixels";
        case SF_INT_16:
            return "16-bit integer pixels";
        case SF_INT_32:
            return "32-bit integer pixels";
        case SF_UINT_8:
            return "8-bit unsigned integer pixels";
        case SF_UINT_16:
            return "16-bit unsigned integer pixels";
        case SF_UINT_32:
            return "32-bit unsigned integer pixels";
        case SF_FLOAT:
            return "32-bit floating point pixels";
        case SF_DOUBLE:
            return "64-bit floating point pixels";
        }

        return "Unknown";
    }

    const char* Image::getSizeAndColor() const
    {
        static char tmp[50];

        sprintf(tmp, "%dx%d %s image", getWidth(), getHeight(),
                isColor() ? "Color" : "Grayscale");

        return tmp;
    }

}