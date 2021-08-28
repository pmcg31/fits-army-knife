#include "pixstfparms.h"

namespace ELS
{

    PixSTFParms::PixSTFParms()
        : _mBal{0.5, 0.5, 0.5},
          _sClip{0.0, 0.0, 0.0},
          _hClip{1.0, 1.0, 1.0},
          _sExp{0.0, 0.0, 0.0},
          _hExp{1.0, 1.0, 1.0}
    {
    }

    PixSTFParms::PixSTFParms(const PixSTFParms& copyFrom)
        : _mBal{copyFrom._mBal[0], copyFrom._mBal[1], copyFrom._mBal[2]},
          _sClip{copyFrom._sClip[0], copyFrom._sClip[1], copyFrom._sClip[2]},
          _hClip{copyFrom._hClip[0], copyFrom._hClip[1], copyFrom._hClip[2]},
          _sExp{copyFrom._sExp[0], copyFrom._sExp[1], copyFrom._sExp[2]},
          _hExp{copyFrom._hExp[0], copyFrom._hExp[1], copyFrom._hExp[2]}
    {
    }

    PixSTFParms& PixSTFParms::operator=(const PixSTFParms& rhs)
    {
        for (int chan = 0; chan < 3; chan++)
        {
            _mBal[chan] = rhs._mBal[chan];
            _sClip[chan] = rhs._sClip[chan];
            _hClip[chan] = rhs._hClip[chan];
            _sExp[chan] = rhs._sExp[chan];
            _hExp[chan] = rhs._hExp[chan];
        }

        return *this;
    }

    bool PixSTFParms::operator==(const PixSTFParms& rhs) const
    {
        for (int chan = 0; chan < 3; chan++)
        {
            if (_mBal[chan] != rhs._mBal[chan])
            {
                return false;
            }
            if (_sClip[chan] != rhs._sClip[chan])
            {
                return false;
            }
            if (_hClip[chan] != rhs._hClip[chan])
            {
                return false;
            }
            if (_sExp[chan] != rhs._sExp[chan])
            {
                return false;
            }
            if (_hExp[chan] != rhs._hExp[chan])
            {
                return false;
            }
        }

        return true;
    }

    bool PixSTFParms::operator!=(const PixSTFParms& rhs) const
    {
        return !(*this == rhs);
    }

    double PixSTFParms::getMBal(int chan /* = 0 */) const
    {
        return _mBal[chan];
    }

    double PixSTFParms::getSClip(int chan /* = 0 */) const
    {
        return _sClip[chan];
    }

    double PixSTFParms::getHClip(int chan /* = 0 */) const
    {
        return _hClip[chan];
    }

    double PixSTFParms::getSExp(int chan /* = 0 */) const
    {
        return _sExp[chan];
    }

    double PixSTFParms::getHExp(int chan /* = 0 */) const
    {
        return _hExp[chan];
    }

    void PixSTFParms::getAll(double* mBal,
                             double* sClip,
                             double* hClip,
                             double* sExp,
                             double* hExp,
                             int chan /* = 0 */) const
    {
        *mBal = _mBal[chan];
        *sClip = _sClip[chan];
        *hClip = _hClip[chan];
        *sExp = _sExp[chan];
        *hExp = _hExp[chan];
    }

    void PixSTFParms::setMBal(double mBal, int chan /* = 0 */)
    {
        _mBal[chan] = mBal;
    }

    void PixSTFParms::setSClip(double sClip, int chan /* = 0 */)
    {
        _sClip[chan] = sClip;
    }

    void PixSTFParms::setHClip(double hClip, int chan /* = 0 */)
    {
        _hClip[chan] = hClip;
    }

    void PixSTFParms::setSExp(double sExp, int chan /* = 0 */)
    {
        _sExp[chan] = sExp;
    }

    void PixSTFParms::setHExp(double hExp, int chan /* = 0 */)
    {
        _hExp[chan] = hExp;
    }
}