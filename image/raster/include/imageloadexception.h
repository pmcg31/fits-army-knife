#pragma once

#include <exception>

namespace ELS
{

    class ImageLoadException : public std::exception
    {
    public:
        ImageLoadException(const char* errText);
        virtual ~ImageLoadException();

        virtual const char* getErrText() const;

    protected:
        ImageLoadException();

    protected:
        static const int g_bufSize = 200;

    protected:
        char _errText[g_bufSize];
    };

}
