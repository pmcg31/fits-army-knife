#pragma once

#include "fitspixelvisitor.h"

template <typename PixelT>
class StatisticsVisitor : public ELS::FITSPixelVisitor<PixelT>
{
public:
    StatisticsVisitor();
    ~StatisticsVisitor();

    PixelT getMinVal(int chan = 0) const;
    PixelT getMaxVal(int chan = 0) const;
    PixelT getMedianVal(int chan = 0) const;
    PixelT getMeanVal(int chan = 0) const;

public:
    virtual void pixelFormat(ELS::FITS::PixelFormat pf) override;
    virtual void dimensions(int width, int height) override;
    virtual void pixel(int x, int y, const PixelT *pixelVals) override;
    virtual void done() override;

private:
    int _width;
    int _height;
    int _rIdx;
    int _gIdx;
    int _bIdx;
    bool _isFirstPixel;
    PixelT _minVal[3];
    PixelT _maxVal[3];
    PixelT _medVal[3];
    PixelT _meanVal[3];
};

template <typename PixelT>
StatisticsVisitor<PixelT>::StatisticsVisitor()
    : _width(0),
      _height(0),
      _rIdx(0),
      _gIdx(-1),
      _bIdx(-1),
      _isFirstPixel(true),
      _minVal{0, 0, 0},
      _maxVal{0, 0, 0},
      _medVal{0, 0, 0},
      _meanVal{0, 0, 0}
{
}

template <typename PixelT>
StatisticsVisitor<PixelT>::~StatisticsVisitor()
{
}

template <typename PixelT>
PixelT StatisticsVisitor<PixelT>::getMinVal(int chan /* = 0 */) const
{
    return _minVal[chan];
}

template <typename PixelT>
PixelT StatisticsVisitor<PixelT>::getMaxVal(int chan /* = 0 */) const
{
    return _maxVal[chan];
}

template <typename PixelT>
PixelT StatisticsVisitor<PixelT>::getMedianVal(int chan /* = 0 */) const
{
    return _medVal[chan];
}

template <typename PixelT>
PixelT StatisticsVisitor<PixelT>::getMeanVal(int chan /* = 0 */) const
{
    return _meanVal[chan];
}

template <typename PixelT>
void StatisticsVisitor<PixelT>::pixelFormat(ELS::FITS::PixelFormat pf)
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

    _isFirstPixel = true;
}

template <typename PixelT>
void StatisticsVisitor<PixelT>::dimensions(int width, int height)
{
    _width = width;
    _height = height;
}

template <typename PixelT>
void StatisticsVisitor<PixelT>::pixel(int x, int y, const PixelT *pixelVals)
{
    (void)x;
    (void)y;

    if (_gIdx != -1)
    {
        // Color
        if (_isFirstPixel)
        {
            _isFirstPixel = false;
            _minVal[0] = pixelVals[_rIdx];
            _minVal[1] = pixelVals[_gIdx];
            _minVal[2] = pixelVals[_bIdx];
            _maxVal[0] = pixelVals[_rIdx];
            _maxVal[1] = pixelVals[_gIdx];
            _maxVal[2] = pixelVals[_bIdx];
        }
        else
        {
            if (pixelVals[_rIdx] < _minVal[0])
            {
                _minVal[0] = pixelVals[_rIdx];
            }
            if (pixelVals[_gIdx] < _minVal[1])
            {
                _minVal[1] = pixelVals[_gIdx];
            }
            if (pixelVals[_bIdx] < _minVal[2])
            {
                _minVal[2] = pixelVals[_bIdx];
            }
            if (pixelVals[_rIdx] > _maxVal[0])
            {
                _maxVal[0] = pixelVals[_rIdx];
            }
            if (pixelVals[_gIdx] > _maxVal[1])
            {
                _maxVal[1] = pixelVals[_gIdx];
            }
            if (pixelVals[_bIdx] > _maxVal[2])
            {
                _maxVal[2] = pixelVals[_bIdx];
            }
        }
    }
    else
    {
        // Grayscale
        if (_isFirstPixel)
        {
            _isFirstPixel = false;
            _minVal[0] = pixelVals[0];
            _maxVal[0] = pixelVals[0];
        }
        else
        {
            if (pixelVals[0] < _minVal[0])
            {
                _minVal[0] = pixelVals[0];
            }
            if (pixelVals[0] > _maxVal[0])
            {
                _maxVal[0] = pixelVals[0];
            }
        }
    }
}

template <typename PixelT>
void StatisticsVisitor<PixelT>::done()
{
}
