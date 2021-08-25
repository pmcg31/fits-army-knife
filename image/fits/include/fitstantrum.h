#pragma once

#include "fitsexception.h"

namespace ELS
{

    class FITSTantrum : public FITSException
    {
    public:
        FITSTantrum(int status);
        virtual ~FITSTantrum() override;

        virtual int getStatus() const;

    protected:
        int _status;
    };

}
