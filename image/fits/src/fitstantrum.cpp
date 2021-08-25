#include <fitsio.h>

#include "fitstantrum.h"

namespace ELS
{

    FITSTantrum::FITSTantrum(int status)
        : FITSException(), _status(status)
    {
        fits_get_errstatus(_status, _errText);
    }

    FITSTantrum::~FITSTantrum() {}

    /* virtual */
    int FITSTantrum::getStatus() const
    {
        return _status;
    }

}
