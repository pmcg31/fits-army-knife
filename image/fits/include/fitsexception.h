#pragma once

#include "imageloadexception.h"

namespace ELS
{

    class FITSException : public ImageLoadException
    {
    public:
        FITSException(const char* errText);
        virtual ~FITSException();

        virtual const char* getErrText() const;

    protected:
        FITSException();

    protected:
        static const int g_bufSize = 200;

    protected:
        char _errText[g_bufSize];
    };

}
