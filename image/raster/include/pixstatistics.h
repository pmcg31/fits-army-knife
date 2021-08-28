#pragma once

#include <algorithm>
#include <inttypes.h>

#include "pixutils.h"
#include "pixstfparms.h"

namespace ELS
{

    template <typename PixelT>
    class PixStatistics
    {
    public:
        PixStatistics();
        PixStatistics(const PixelT *minVal,
                      const PixelT *maxVal,
                      const PixelT *medVal,
                      const PixelT *meanval,
                      const PixelT *madn);
        PixStatistics(const PixStatistics &copyFrom);

        void getMinVal(PixelT *minVal) const;
        PixelT getMinVal(int chan = 0) const;

        void getMaxVal(PixelT *maxVal) const;
        PixelT getMaxVal(int chan = 0) const;

        void getMedVal(PixelT *medVal) const;
        PixelT getMedVal(int chan = 0) const;

        void getMeanVal(PixelT *meanVal) const;
        PixelT getMeanVal(int chan = 0) const;

        void getMADN(PixelT *madn) const;
        PixelT getMADN(int chan = 0) const;

        PixSTFParms getStretchParameters() const;

        void setMinVal(int chan, PixelT minVal);
        void setMaxVal(int chan, PixelT maxVal);
        void setMedVal(int chan, PixelT medVal);
        void setMeanVal(int chan, PixelT meanVal);
        void setMADN(int chan, PixelT madn);

    private:
        static PixSTFParms getStretchParameters(const int8_t *madn,
                                                const int8_t *medVal);
        static PixSTFParms getStretchParameters(const int16_t *madn,
                                                const int16_t *medVal);
        static PixSTFParms getStretchParameters(const int32_t *madn,
                                                const int32_t *medVal);
        static PixSTFParms getStretchParameters(const uint8_t *madn,
                                                const uint8_t *medVal);
        static PixSTFParms getStretchParameters(const uint16_t *madn,
                                                const uint16_t *medVal);
        static PixSTFParms getStretchParameters(const uint32_t *madn,
                                                const uint32_t *medVal);
        static PixSTFParms getStretchParameters(const float *madn,
                                                const float *medVal);
        static PixSTFParms getStretchParameters(const double *madn,
                                                const double *medVal);

    private:
        PixelT _minVal[3];
        PixelT _maxVal[3];
        PixelT _medVal[3];
        PixelT _meanVal[3];
        PixelT _madn[3];

    private:
        static constexpr double g_B = 0.25;
        static constexpr double g_C = -2.8;
    };

    template <typename PixelT>
    PixStatistics<PixelT>::PixStatistics()
        : _minVal{0, 0, 0},
          _maxVal{0, 0, 0},
          _medVal{0, 0, 0},
          _meanVal{0, 0, 0},
          _madn{0, 0, 0}
    {
    }

    template <typename PixelT>
    PixStatistics<PixelT>::PixStatistics(const PixelT *minVal,
                                         const PixelT *maxVal,
                                         const PixelT *medVal,
                                         const PixelT *meanVal,
                                         const PixelT *madn)
        : _minVal{minVal[0], minVal[1], minVal[2]},
          _maxVal{maxVal[0], maxVal[1], maxVal[2]},
          _medVal{medVal[0], medVal[1], medVal[2]},
          _meanVal{meanVal[0], meanVal[1], meanVal[2]},
          _madn{madn[0], madn[1], madn[2]}
    {
    }

    template <typename PixelT>
    PixStatistics<PixelT>::PixStatistics(const PixStatistics &copyFrom)
        : _minVal{copyFrom._minVal[0], copyFrom._minVal[1], copyFrom._minVal[2]},
          _maxVal{copyFrom._maxVal[0], copyFrom._maxVal[1], copyFrom._maxVal[2]},
          _medVal{copyFrom._medVal[0], copyFrom._medVal[1], copyFrom._medVal[2]},
          _meanVal{copyFrom._meanVal[0], copyFrom._meanVal[1], copyFrom._meanVal[2]},
          _madn{copyFrom._madn[0], copyFrom._madn[1], copyFrom._madn[2]}
    {
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::getMinVal(PixelT *minVal) const
    {
        for (int chan = 0; chan < 3; chan++)
        {
            minVal[chan] = _minVal[chan];
        }
    }

    template <typename PixelT>
    PixelT PixStatistics<PixelT>::getMinVal(int chan /* = 0 */) const
    {
        return _minVal[chan];
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::getMaxVal(PixelT *maxVal) const
    {
        for (int chan = 0; chan < 3; chan++)
        {
            maxVal[chan] = _maxVal[chan];
        }
    }

    template <typename PixelT>
    PixelT PixStatistics<PixelT>::getMaxVal(int chan /* = 0 */) const
    {
        return _maxVal[chan];
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::getMedVal(PixelT *medVal) const
    {
        for (int chan = 0; chan < 3; chan++)
        {
            medVal[chan] = _medVal[chan];
        }
    }

    template <typename PixelT>
    PixelT PixStatistics<PixelT>::getMedVal(int chan /* = 0 */) const
    {
        return _medVal[chan];
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::getMeanVal(PixelT *meanVal) const
    {
        for (int chan = 0; chan < 3; chan++)
        {
            meanVal[chan] = _meanVal[chan];
        }
    }

    template <typename PixelT>
    PixelT PixStatistics<PixelT>::getMeanVal(int chan /* = 0 */) const
    {
        return _meanVal[chan];
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::getMADN(PixelT *madn) const
    {
        for (int chan = 0; chan < 3; chan++)
        {
            madn[chan] = _madn[chan];
        }
    }

    template <typename PixelT>
    PixelT PixStatistics<PixelT>::getMADN(int chan /* = 0 */) const
    {
        return _madn[chan];
    }

    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters() const
    {
        return getStretchParameters(_madn,
                                    _medVal);
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::setMinVal(int chan, PixelT minVal)
    {
        _minVal[chan] = minVal;
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::setMaxVal(int chan, PixelT maxVal)
    {
        _maxVal[chan] = maxVal;
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::setMedVal(int chan, PixelT medVal)
    {
        _medVal[chan] = medVal;
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::setMeanVal(int chan, PixelT meanVal)
    {
        _meanVal[chan] = meanVal;
    }

    template <typename PixelT>
    void PixStatistics<PixelT>::setMADN(int chan, PixelT madn)
    {
        _madn[chan] = madn;
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const int8_t *madn,
                                                            const int8_t *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan] / PixUtils::g_u8Max;
            tmpMedVal[chan] = (double)medVal[chan] / PixUtils::g_u8Max;
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const int16_t *madn,
                                                            const int16_t *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan] / PixUtils::g_u16Max;
            tmpMedVal[chan] = (double)medVal[chan] / PixUtils::g_u16Max;
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const int32_t *madn,
                                                            const int32_t *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan] / PixUtils::g_u32Max;
            tmpMedVal[chan] = (double)medVal[chan] / PixUtils::g_u32Max;
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const uint8_t *madn,
                                                            const uint8_t *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan] / PixUtils::g_u8Max;
            tmpMedVal[chan] = (double)medVal[chan] / PixUtils::g_u8Max;
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const uint16_t *madn,
                                                            const uint16_t *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan] / PixUtils::g_u16Max;
            tmpMedVal[chan] = (double)medVal[chan] / PixUtils::g_u16Max;
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const uint32_t *madn,
                                                            const uint32_t *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan] / PixUtils::g_u32Max;
            tmpMedVal[chan] = (double)medVal[chan] / PixUtils::g_u32Max;
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const float *madn,
                                                            const float *medVal)
    {
        double tmpMADN[3];
        double tmpMedVal[3];
        for (int chan = 0; chan < 3; chan++)
        {
            tmpMADN[chan] = (double)madn[chan];
            tmpMedVal[chan] = (double)medVal[chan];
        }

        return getStretchParameters(tmpMADN, tmpMedVal);
    }

    /* static */
    template <typename PixelT>
    PixSTFParms PixStatistics<PixelT>::getStretchParameters(const double *madn,
                                                            const double *medVal)
    {
        PixSTFParms stfParms;

        double tmpSClip = 0.0;
        double tmpHClip = 0.0;
        for (int chan = 0; chan < 3; chan++)
        {
            if ((medVal[chan] > 0.5) ||
                (madn[chan] == 0))
            {
                tmpSClip = 0.0;
                stfParms.setSClip(tmpSClip, chan);
            }
            else
            {
                tmpSClip = std::min(1.0,
                                    std::max(0.0,
                                             medVal[chan] + g_C * madn[chan]));
                stfParms.setSClip(tmpSClip,
                                  chan);
            }

            if ((medVal[chan] <= 0.5) ||
                (madn[chan] == 0))
            {
                tmpHClip = 1.0;
                stfParms.setHClip(tmpHClip, chan);
            }
            else
            {
                tmpHClip = std::min(1.0,
                                    std::max(0.0,
                                             medVal[chan] - g_C * madn[chan]));
                stfParms.setHClip(tmpHClip,
                                  chan);
            }

            if (medVal[chan] <= 0.5)
            {
                double mBal = PixUtils::midtonesTransferFunc(medVal[chan] - tmpSClip, g_B);
                stfParms.setMBal(mBal, chan);
            }
            else
            {
                stfParms.setMBal(PixUtils::midtonesTransferFunc(g_B, tmpHClip - medVal[chan]), chan);
            }

            stfParms.setSExp(0.0, chan);
            stfParms.setHExp(1.0, chan);
        }

        return stfParms;
    }

}
