#pragma once

#include <exception>

namespace ELS
{

    class FITSException : public std::exception
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
