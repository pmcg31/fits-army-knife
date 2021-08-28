#include <string.h>

#include "pixelvisitortypemismatch.h"

namespace ELS
{

    PixelVisitorTypeMismatch::PixelVisitorTypeMismatch(const char *errText)
    {
        strncpy(_errText, errText, g_bufSize - 1);
        _errText[g_bufSize - 1] = 0;
    }

    /* virtual */
    PixelVisitorTypeMismatch::~PixelVisitorTypeMismatch() {}

    /* virtual */
    const char *PixelVisitorTypeMismatch::getErrText() const
    {
        return _errText;
    }

    /* protected */
    PixelVisitorTypeMismatch::PixelVisitorTypeMismatch()
    {
        _errText[g_bufSize - 1] = 0;
    }

}
