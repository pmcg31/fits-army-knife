#pragma once

#include "pixelvisitor.h"
#include "rastertypes.h"

namespace ELS
{

    class Image
    {
    public:
        static Image* load(const char* filename);
        static bool isSupportedFile(const char* filename,
                                    char* error = 0);

    public:
        virtual ~Image();

        virtual bool isColor() const = 0;

        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;

        virtual RasterFormat getRasterFormat() const = 0;
        virtual SampleFormat getSampleFormat() const = 0;

        virtual void visitPixels(PixelVisitor* visitor) const = 0;

        const char* getImageType() const;
        const char* getSizeAndColor() const;

    private:
        enum FileType
        {
            FT_UNKNOWN,
            FT_FITS,
            FT_XISF
        };

    private:
        struct ExtInfo
        {
            const char* ext;
            const int extLen;
            const FileType fileType;
        };

    private:
        struct MagicInfo
        {
            const char* magic;
            const size_t magicLen;
            const FileType fileType;
        };

    private:
        static FileType fileTypeFromFilename(const char* filename);
        static bool checkMagic(const char* filename,
                               MagicInfo* magic,
                               char* error = 0);

    private:
        static const char* g_fileTypeStr[];
        static ExtInfo g_extInfo[];
        static MagicInfo g_fitsMagic;
        static MagicInfo g_xisfMagic;
        static const int g_maxMagicLen;
    };

}