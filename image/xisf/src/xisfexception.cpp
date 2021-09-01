#include <string.h>

#include "xisfexception.h"

namespace ELS
{

    XISFException::XISFException(const char* errText)
    {
        strncpy(_errText, errText, g_bufSize - 1);
        _errText[g_bufSize - 1] = 0;
    }

    /* virtual */
    XISFException::~XISFException() {}

    /* virtual */
    const char* XISFException::getErrText() const
    {
        return _errText;
    }

    /* protected */
    XISFException::XISFException()
    {
        _errText[g_bufSize - 1] = 0;
    }

}
