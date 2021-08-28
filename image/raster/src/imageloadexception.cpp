#include <string.h>

#include "imageloadexception.h"

namespace ELS
{

    ImageLoadException::ImageLoadException(const char* errText)
    {
        strncpy(_errText, errText, g_bufSize - 1);
        _errText[g_bufSize - 1] = 0;
    }

    /* virtual */
    ImageLoadException::~ImageLoadException() {}

    /* virtual */
    const char* ImageLoadException::getErrText() const
    {
        return _errText;
    }

    /* protected */
    ImageLoadException::ImageLoadException()
    {
        _errText[g_bufSize - 1] = 0;
    }

}
