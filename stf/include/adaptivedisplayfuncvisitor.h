#pragma once

#include <algorithm>
#include <inttypes.h>
#include "pixutils.h"
#include "fitspixelvisitor.h"

namespace ELS
{

    template <typename PixelT>
    class AdaptiveDisplayFuncVisitor : public ELS::FITSPixelVisitor<PixelT>
    {
    public:
        AdaptiveDisplayFuncVisitor(PixelT medianVals[3]);
        ~AdaptiveDisplayFuncVisitor();

        void getMADN(double *madn) const;
        void getSClip(double *sClip) const;
        void getHClip(double *hClip) const;
        void getMBal(double *mBal) const;

    public:
        virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void pixel(int x, int y, const PixelT *pixelVals) override;
        virtual void done() override;

    private:
        PixelT _medVal[3];
        int _rIdx;
        int _gIdx;
        int _bIdx;
        int64_t _pixelCount;
        uint32_t *_histogram;
        double _madn[3];
        double _sClip[3];
        double _hClip[3];
        double _mBal[3];
    };

    template <typename PixelT>
    AdaptiveDisplayFuncVisitor<PixelT>::AdaptiveDisplayFuncVisitor(PixelT medianVals[3])
        : _medVal{medianVals[0], medianVals[1], medianVals[2]},
          _rIdx(0),
          _gIdx(-1),
          _bIdx(-1),
          _pixelCount(0),
          _histogram(0),
          _madn{0, 0, 0},
          _sClip{0, 0, 0},
          _hClip{0, 0, 0},
          _mBal{0, 0, 0}
    {
    }

    template <typename PixelT>
    AdaptiveDisplayFuncVisitor<PixelT>::~AdaptiveDisplayFuncVisitor()
    {
        if (_histogram != 0)
        {
            delete[] _histogram;
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::getMADN(double *madn) const
    {
        madn[0] = _madn[0];
        if (_gIdx != -1)
        {
            madn[1] = _madn[1];
            madn[2] = _madn[2];
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::getSClip(double *sClip) const
    {
        sClip[0] = _sClip[0];
        if (_gIdx != -1)
        {
            sClip[1] = _sClip[1];
            sClip[2] = _sClip[2];
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::getHClip(double *hClip) const
    {
        hClip[0] = _hClip[0];
        if (_gIdx != -1)
        {
            hClip[1] = _hClip[1];
            hClip[2] = _hClip[2];
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::getMBal(double *mBal) const
    {
        mBal[0] = _mBal[0];
        if (_gIdx != -1)
        {
            mBal[1] = _mBal[1];
            mBal[2] = _mBal[2];
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::pixelFormat(ELS::FITS::PixelFormat pf)
    {
        switch (pf)
        {
        case ELS::FITS::PF_GRAY:
            _rIdx = 0;
            _gIdx = -1;
            _bIdx = -1;
            break;
        case ELS::FITS::PF_RGB:
            _rIdx = 0;
            _gIdx = 1;
            _bIdx = 2;
            break;
        case ELS::FITS::PF_BGR:
            _rIdx = 2;
            _gIdx = 1;
            _bIdx = 0;
            break;
        }

        _pixelCount = 0;

        int totalHistogramPoints = PixUtils::g_histogramPoints;
        if (_gIdx != -1)
        {
            totalHistogramPoints *= 3;
        }

        _histogram = new uint32_t[totalHistogramPoints];
        for (int i = 0; i < PixUtils::g_histogramPoints; i++)
        {
            _histogram[i] = 0;
            if (_gIdx != -1)
            {
                _histogram[PixUtils::g_histogramPoints + i] = 0;
                _histogram[PixUtils::g_histogramPoints * 2 + i] = 0;
            }
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::dimensions(int width, int height)
    {
        (void)width;
        (void)height;
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::pixel(int x, int y, const PixelT *pixelVals)
    {
        (void)x;
        (void)y;

        _pixelCount++;

        if (_gIdx != -1)
        {
            // Color
            PixelT rVal = pixelVals[_rIdx] > _medVal[0] ? pixelVals[_rIdx] - _medVal[0] : _medVal[0] - pixelVals[_rIdx];
            PixelT gVal = pixelVals[_gIdx] > _medVal[1] ? pixelVals[_gIdx] - _medVal[1] : _medVal[1] - pixelVals[_gIdx];
            PixelT bVal = pixelVals[_bIdx] > _medVal[2] ? pixelVals[_bIdx] - _medVal[2] : _medVal[2] - pixelVals[_bIdx];

            _histogram[PixUtils::convertRangeToHist(rVal)]++;
            _histogram[PixUtils::g_histogramPoints + PixUtils::convertRangeToHist(gVal)]++;
            _histogram[PixUtils::g_histogramPoints * 2 + PixUtils::convertRangeToHist(bVal)]++;
        }
        else
        {
            PixelT val = pixelVals[0] > _medVal[0] ? pixelVals[0] - _medVal[0] : _medVal[0] - pixelVals[0];

            _histogram[PixUtils::convertRangeToHist(val)]++;
        }
    }

    template <typename PixelT>
    void AdaptiveDisplayFuncVisitor<PixelT>::done()
    {
        const double madnConstant = 1.4826;
        const double B = 0.25;
        const double C = -2.8;

        int numChan = _gIdx != -1 ? 3 : 1;

        int64_t halfway = _pixelCount / 2;
        int64_t pointCount[3] = {0, 0, 0};
        bool done = false;
        for (int i = 0; (!done) && (i < PixUtils::g_histogramPoints); i++)
        {
            done = true;

            if (pointCount[0] < halfway)
            {
                pointCount[0] += _histogram[i];
                if (pointCount[0] >= halfway)
                {
                    double dval = 0.0;
                    PixUtils::convertRangeFromHist(i, &dval);
                    _madn[0] = dval * madnConstant;
                }
                else
                {
                    done = false;
                }
            }

            if (numChan == 3)
            {
                if (pointCount[1] < halfway)
                {
                    pointCount[1] += _histogram[PixUtils::g_histogramPoints + i];
                    if (pointCount[1] >= halfway)
                    {
                        double dval = 0.0;
                        PixUtils::convertRangeFromHist(i, &dval);
                        _madn[1] = dval * madnConstant;
                    }
                    else
                    {
                        done = false;
                    }
                }

                if (pointCount[2] < halfway)
                {
                    pointCount[2] += _histogram[PixUtils::g_histogramPoints * 2 + i];
                    if (pointCount[2] >= halfway)
                    {
                        double dval = 0.0;
                        PixUtils::convertRangeFromHist(i, &dval);
                        _madn[2] = dval * madnConstant;
                    }
                    else
                    {
                        done = false;
                    }
                }
            }
        }

        for (int chan = 0; chan < numChan; chan++)
        {
            uint16_t tmp = PixUtils::convertRangeToHist(_medVal[chan]);
            double medVal = 0.0;
            PixUtils::convertRangeFromHist(tmp, &medVal);

            if ((medVal > 0.5) || (_madn[chan] == 0))
            {
                _sClip[chan] = 0;
            }
            else
            {
                _sClip[chan] = std::min(1.0, std::max(0.0, medVal + C * _madn[chan]));
            }
            if ((medVal < 0.5) || (_madn[chan] == 0))
            {
                _hClip[chan] = 1;
            }
            else
            {
                _hClip[chan] = std::min(1.0, std::max(0.0, medVal - C * _madn[chan]));
            }
            if (medVal < 0.5)
            {
                _mBal[chan] = PixUtils::midtonesTransferFunc(medVal - _sClip[chan], B);
            }
            else
            {
                _mBal[chan] = PixUtils::midtonesTransferFunc(B, _hClip[chan] - medVal);
            }
        }
    }

}