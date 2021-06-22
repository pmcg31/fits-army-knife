#include <string.h>

#include "fitsexception.h"

namespace ELS
{

    FITSException::FITSException(const char *errText)
    {
        strncpy(_errText, errText, g_bufSize - 1);
        _errText[g_bufSize - 1] = 0;
    }

    /* virtual */
    FITSException::~FITSException() {}

    /* virtual */
    const char *FITSException::getErrText() const
    {
        return _errText;
    }

    /* protected */
    FITSException::FITSException()
    {
        _errText[g_bufSize - 1] = 0;
    }

}
