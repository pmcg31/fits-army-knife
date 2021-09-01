#pragma once

#include "imageloadexception.h"

namespace ELS
{

    class XISFException : public ImageLoadException
    {
    public:
        XISFException(const char* errText);
        virtual ~XISFException();

        virtual const char* getErrText() const;

    protected:
        XISFException();

    protected:
        static const int g_bufSize = 200;

    protected:
        char _errText[g_bufSize];
    };

}
