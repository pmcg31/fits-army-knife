#pragma once

#include <exception>

namespace ELS
{

    class PixelVisitorTypeMismatch : public std::exception
    {
    public:
        PixelVisitorTypeMismatch(const char* errText);
        virtual ~PixelVisitorTypeMismatch();

        virtual const char* getErrText() const;

    protected:
        PixelVisitorTypeMismatch();

    protected:
        static const int g_bufSize = 200;

    protected:
        char _errText[g_bufSize];
    };

}
