#include "pixutils.h"
#include "statisticsvisitor.h"
#include "imagefilelistitem.h"

ImageFileListItem::ImageFileListItem()
    : ImageFileListItem("")
{
}

ImageFileListItem::ImageFileListItem(QString absolutePath,
                                     ELS::Image::FileType fileType /* = ELS::Image::FT_UNKNOWN */,
                                     bool isValidated /* = false */)
    : _absolutePath(absolutePath),
      _fileType(fileType),
      _isValidated(isValidated),
      _isLoaded(false),
      _showStretched(false),
      _image(),
      _stfParms(),
      _min(),
      _mean(),
      _median(),
      _max(),
      _numHistogramPoints(0),
      _histogram(),
      _stfLUT(0),
      _identityLUT(0),
      _lutInUse(0),
      _qiData(),
      _qi()
{
}

ImageFileListItem::~ImageFileListItem() {}

QString ImageFileListItem::absolutePath() const
{
    return _absolutePath;
}

bool ImageFileListItem::isValidated() const
{
    return _isValidated;
}

bool ImageFileListItem::isLoaded() const
{
    return _isLoaded;
}

bool ImageFileListItem::showStretched() const
{
    return _showStretched;
}

bool ImageFileListItem::isColor() const
{
    return _image->isColor();
}

QString ImageFileListItem::getMin() const
{
    return _min;
}

QString ImageFileListItem::getMean() const
{
    return _mean;
}

QString ImageFileListItem::getMedian() const
{
    return _median;
}

QString ImageFileListItem::getMax() const
{
    return _max;
}

int ImageFileListItem::getNumHistogramPoints() const
{
    return _numHistogramPoints;
}

std::shared_ptr<const uint32_t[]> ImageFileListItem::getHistogram() const
{
    return _histogram;
}

std::shared_ptr<const QImage> ImageFileListItem::getQImage() const
{
    return _qi;
}

void ImageFileListItem::setValidated(bool isValidated)
{
    _isValidated = isValidated;
}

void ImageFileListItem::setShowStretched(bool showStretched)
{
    if (_showStretched != showStretched)
    {
        _showStretched = showStretched;
        if (_showStretched)
        {
            _lutInUse = _stfLUT;
        }
        else
        {
            _lutInUse = _identityLUT;
        }

        ToQImageVisitor visitor(_stfParms, _lutInUse, _numHistogramPoints);
        _image->visitPixels(&visitor);
        _qiData = visitor.getImageData();
        _qi = visitor.getImage();
    }
}

void ImageFileListItem::load()
{
    if (!_isLoaded)
    {
        QByteArray ba = _absolutePath.toLocal8Bit();
        const char* filename = ba.data();

        if (!_isValidated)
        {
            char error[2048];
            _fileType = ELS::Image::isSupportedFile(filename, error);

            if (_fileType == ELS::Image::FT_UNKNOWN)
            {
                printf("Failed to load image file '%s': %s", filename, error);
                return;
            }

            _isValidated = true;
        }

        _image.reset(ELS::Image::load(filename, _fileType));

        calculateStatistics();
        calculateLUTs();

        ToQImageVisitor visitor(_stfParms, _lutInUse, _numHistogramPoints);
        _image->visitPixels(&visitor);
        _qiData = visitor.getImageData();
        _qi = visitor.getImage();

        _isLoaded = true;
    }
}

void ImageFileListItem::streamTo(QDataStream& out) const
{
    out << _absolutePath
        << _fileType
        << _isValidated;
}

void ImageFileListItem::streamFrom(QDataStream& in)
{
    in >> _absolutePath >> _fileType >> _isValidated;

    _isLoaded = false;
    _showStretched = false;

    _image.reset();
}

bool ImageFileListItem::operator==(const ImageFileListItem& rhs) const
{
    return _absolutePath == rhs._absolutePath;
}

bool ImageFileListItem::operator!=(const ImageFileListItem& rhs) const
{
    return _absolutePath != rhs._absolutePath;
}

bool ImageFileListItem::operator<(const ImageFileListItem& rhs) const
{
    return _absolutePath < rhs._absolutePath;
}

bool ImageFileListItem::operator>(const ImageFileListItem& rhs) const
{
    return _absolutePath > rhs._absolutePath;
}

bool ImageFileListItem::operator<=(const ImageFileListItem& rhs) const
{
    return _absolutePath <= rhs._absolutePath;
}

bool ImageFileListItem::operator>=(const ImageFileListItem& rhs) const
{
    return _absolutePath >= rhs._absolutePath;
}

void ImageFileListItem::calculateStatistics()
{
    bool isColor = _image->isColor();

    QString gMinF(" min: %1 ");
    QString gMeanF(" mean: %1 ");
    QString gMedF(" median: %1 ");
    QString gMaxF(" max: %1 ");
    QString cMinF(" min: %1 | %2 | %3 ");
    QString cMeanF(" mean: %1 | %2 | %3 ");
    QString cMedF(" median: %1 | %2 | %3 ");
    QString cMaxF(" max: %1 | %2 | %3 ");

    switch (_image->getSampleFormat())
    {
    case ELS::SF_UINT_8:
    {
        ELS::StatisticsVisitor<uint8_t> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<uint8_t> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal());
            _mean = gMeanF.arg(localStats.getMeanVal());
            _median = gMedF.arg(localStats.getMedVal());
            _max = gMaxF.arg(localStats.getMaxVal());
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0))
                       .arg(localStats.getMinVal(1))
                       .arg(localStats.getMinVal(2));
            _mean = cMeanF.arg(localStats.getMeanVal(0))
                        .arg(localStats.getMeanVal(1))
                        .arg(localStats.getMeanVal(2));
            _median = cMedF.arg(localStats.getMedVal(0))
                          .arg(localStats.getMedVal(1))
                          .arg(localStats.getMedVal(2));
            _max = cMaxF.arg(localStats.getMaxVal(0))
                       .arg(localStats.getMaxVal(1))
                       .arg(localStats.getMaxVal(2));
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_UINT_16:
    {
        ELS::StatisticsVisitor<uint16_t> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<uint16_t> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal());
            _mean = gMeanF.arg(localStats.getMeanVal());
            _median = gMedF.arg(localStats.getMedVal());
            _max = gMaxF.arg(localStats.getMaxVal());
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0))
                       .arg(localStats.getMinVal(1))
                       .arg(localStats.getMinVal(2));
            _mean = cMeanF.arg(localStats.getMeanVal(0))
                        .arg(localStats.getMeanVal(1))
                        .arg(localStats.getMeanVal(2));
            _median = cMedF.arg(localStats.getMedVal(0))
                          .arg(localStats.getMedVal(1))
                          .arg(localStats.getMedVal(2));
            _max = cMaxF.arg(localStats.getMaxVal(0))
                       .arg(localStats.getMaxVal(1))
                       .arg(localStats.getMaxVal(2));
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_UINT_32:
    {
        ELS::StatisticsVisitor<uint32_t> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<uint32_t> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal());
            _mean = gMeanF.arg(localStats.getMeanVal());
            _median = gMedF.arg(localStats.getMedVal());
            _max = gMaxF.arg(localStats.getMaxVal());
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0))
                       .arg(localStats.getMinVal(1))
                       .arg(localStats.getMinVal(2));
            _mean = cMeanF.arg(localStats.getMeanVal(0))
                        .arg(localStats.getMeanVal(1))
                        .arg(localStats.getMeanVal(2));
            _median = cMedF.arg(localStats.getMedVal(0))
                          .arg(localStats.getMedVal(1))
                          .arg(localStats.getMedVal(2));
            _max = cMaxF.arg(localStats.getMaxVal(0))
                       .arg(localStats.getMaxVal(1))
                       .arg(localStats.getMaxVal(2));
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_INT_8:
    {
        ELS::StatisticsVisitor<int8_t> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<int8_t> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal());
            _mean = gMeanF.arg(localStats.getMeanVal());
            _median = gMedF.arg(localStats.getMedVal());
            _max = gMaxF.arg(localStats.getMaxVal());
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0))
                       .arg(localStats.getMinVal(1))
                       .arg(localStats.getMinVal(2));
            _mean = cMeanF.arg(localStats.getMeanVal(0))
                        .arg(localStats.getMeanVal(1))
                        .arg(localStats.getMeanVal(2));
            _median = cMedF.arg(localStats.getMedVal(0))
                          .arg(localStats.getMedVal(1))
                          .arg(localStats.getMedVal(2));
            _max = cMaxF.arg(localStats.getMaxVal(0))
                       .arg(localStats.getMaxVal(1))
                       .arg(localStats.getMaxVal(2));
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_INT_16:
    {
        ELS::StatisticsVisitor<int16_t> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<int16_t> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal());
            _mean = gMeanF.arg(localStats.getMeanVal());
            _median = gMedF.arg(localStats.getMedVal());
            _max = gMaxF.arg(localStats.getMaxVal());
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0))
                       .arg(localStats.getMinVal(1))
                       .arg(localStats.getMinVal(2));
            _mean = cMeanF.arg(localStats.getMeanVal(0))
                        .arg(localStats.getMeanVal(1))
                        .arg(localStats.getMeanVal(2));
            _median = cMedF.arg(localStats.getMedVal(0))
                          .arg(localStats.getMedVal(1))
                          .arg(localStats.getMedVal(2));
            _max = cMaxF.arg(localStats.getMaxVal(0))
                       .arg(localStats.getMaxVal(1))
                       .arg(localStats.getMaxVal(2));
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_INT_32:
    {
        ELS::StatisticsVisitor<int32_t> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<int32_t> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal());
            _mean = gMeanF.arg(localStats.getMeanVal());
            _median = gMedF.arg(localStats.getMedVal());
            _max = gMaxF.arg(localStats.getMaxVal());
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0))
                       .arg(localStats.getMinVal(1))
                       .arg(localStats.getMinVal(2));
            _mean = cMeanF.arg(localStats.getMeanVal(0))
                        .arg(localStats.getMeanVal(1))
                        .arg(localStats.getMeanVal(2));
            _median = cMedF.arg(localStats.getMedVal(0))
                          .arg(localStats.getMedVal(1))
                          .arg(localStats.getMedVal(2));
            _max = cMaxF.arg(localStats.getMaxVal(0))
                       .arg(localStats.getMaxVal(1))
                       .arg(localStats.getMaxVal(2));
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_FLOAT:
    {
        ELS::StatisticsVisitor<float> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<float> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal(), 0, 'f', 3);
            _mean = gMeanF.arg(localStats.getMeanVal(), 0, 'f', 3);
            _median = gMedF.arg(localStats.getMedVal(), 0, 'f', 3);
            _max = gMaxF.arg(localStats.getMaxVal(), 0, 'f', 3);
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0), 0, 'f', 3)
                       .arg(localStats.getMinVal(1), 0, 'f', 3)
                       .arg(localStats.getMinVal(2), 0, 'f', 3);
            _mean = cMeanF.arg(localStats.getMeanVal(0), 0, 'f', 3)
                        .arg(localStats.getMeanVal(1), 0, 'f', 3)
                        .arg(localStats.getMeanVal(2), 0, 'f', 3);
            _median = cMedF.arg(localStats.getMedVal(0), 0, 'f', 3)
                          .arg(localStats.getMedVal(1), 0, 'f', 3)
                          .arg(localStats.getMedVal(2), 0, 'f', 3);
            _max = cMaxF.arg(localStats.getMaxVal(0), 0, 'f', 3)
                       .arg(localStats.getMaxVal(1), 0, 'f', 3)
                       .arg(localStats.getMaxVal(2), 0, 'f', 3);
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    case ELS::SF_DOUBLE:
    {
        ELS::StatisticsVisitor<double> visitor;
        _image->visitPixels(&visitor);
        ELS::PixStatistics<double> localStats = visitor.getStatistics();
        _stfParms = localStats.getStretchParameters();
        if (!isColor)
        {
            _min = gMinF.arg(localStats.getMinVal(), 0, 'f', 3);
            _mean = gMeanF.arg(localStats.getMeanVal(), 0, 'f', 3);
            _median = gMedF.arg(localStats.getMedVal(), 0, 'f', 3);
            _max = gMaxF.arg(localStats.getMaxVal(), 0, 'f', 3);
        }
        else
        {
            _min = cMinF.arg(localStats.getMinVal(0), 0, 'f', 3)
                       .arg(localStats.getMinVal(1), 0, 'f', 3)
                       .arg(localStats.getMinVal(2), 0, 'f', 3);
            _mean = cMeanF.arg(localStats.getMeanVal(0), 0, 'f', 3)
                        .arg(localStats.getMeanVal(1), 0, 'f', 3)
                        .arg(localStats.getMeanVal(2), 0, 'f', 3);
            _median = cMedF.arg(localStats.getMedVal(0), 0, 'f', 3)
                          .arg(localStats.getMedVal(1), 0, 'f', 3)
                          .arg(localStats.getMedVal(2), 0, 'f', 3);
            _max = cMaxF.arg(localStats.getMaxVal(0), 0, 'f', 3)
                       .arg(localStats.getMaxVal(1), 0, 'f', 3)
                       .arg(localStats.getMaxVal(2), 0, 'f', 3);
        }

        visitor.getHistogramData(&_numHistogramPoints, &_histogram);
    }
    break;
    }

    printf("%s\n", _image->getImageType());
    printf("%s\n", _image->getSizeAndColor());
    fflush(stdout);
}

void ImageFileListItem::calculateLUTs()
{
    int totalHistogramPoints = _numHistogramPoints;
    bool isColor = _image->isColor();
    if (isColor)
    {
        totalHistogramPoints *= 3;
    }
    _stfLUT = new uint8_t[totalHistogramPoints];
    _identityLUT = new uint8_t[totalHistogramPoints];
    _lutInUse = _identityLUT;

    ELS::PixSTFParms stfIdentityParms;
    for (uint16_t i = 0; i < _numHistogramPoints; i++)
    {
        // Only calculate points that are in the image
        if (_histogram[i] != 0)
        {
            if (isColor)
            {
                int chanOffset[3] = {
                    0,
                    _numHistogramPoints,
                    _numHistogramPoints * 2};
                for (int chan = 0; chan < 3; chan++)
                {
                    _stfLUT[chanOffset[chan] + i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                                                &_stfParms) *
                                                              ELS::PixUtils::g_u8Max);
                    _identityLUT[chanOffset[chan] + i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                                                     &stfIdentityParms) *
                                                                   ELS::PixUtils::g_u8Max);
                }
            }
            else
            {
                _stfLUT[i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                         &_stfParms) *
                                       ELS::PixUtils::g_u8Max);
                _identityLUT[i] = (uint8_t)(ELS::PixUtils::screenTransferFunc(i,
                                                                              &stfIdentityParms) *
                                            ELS::PixUtils::g_u8Max);
            }
        }
    }
}

ImageFileListItem::ToQImageVisitor::ToQImageVisitor(ELS::PixSTFParms stfParms,
                                                    uint8_t* lut,
                                                    int lutPoints)
    : _width(0),
      _height(0),
      _pixCount(0),
      _qiData(),
      _stride(0),
      _stfParms(stfParms),
      _qi(),
      _lut(lut),
      _lutPoints(lutPoints)
{
}

ImageFileListItem::ToQImageVisitor::~ToQImageVisitor()
{
}

std::shared_ptr<uint32_t[]> ImageFileListItem::ToQImageVisitor::getImageData()
{
    return _qiData;
}

std::shared_ptr<QImage> ImageFileListItem::ToQImageVisitor::getImage()
{
    return _qi;
}

void ImageFileListItem::ToQImageVisitor::pixelFormat(ELS::PixelFormat pf)
{
    (void)pf;
}

void ImageFileListItem::ToQImageVisitor::dimensions(int width,
                                                    int height)
{
    _width = width;
    _height = height;
    _pixCount = _width * _height;

    _qiData.reset(new uint32_t[_pixCount]);
}

void ImageFileListItem::ToQImageVisitor::rowInfo(int stride)
{
    _stride = stride;
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const int8_t* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const int16_t* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const int32_t* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const uint8_t* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const uint16_t* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const uint32_t* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const float* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowGray(int y,
                                                 const double* k)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(k[dataIdx]);
        uint8_t val = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (val << 16) |
                                 (val << 8) |
                                 (val);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const int8_t* r,
                                                const int8_t* g,
                                                const int8_t* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const int16_t* r,
                                                const int16_t* g,
                                                const int16_t* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const int32_t* r,
                                                const int32_t* g,
                                                const int32_t* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const uint8_t* r,
                                                const uint8_t* g,
                                                const uint8_t* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const uint16_t* r,
                                                const uint16_t* g,
                                                const uint16_t* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const uint32_t* r,
                                                const uint32_t* g,
                                                const uint32_t* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const float* r,
                                                const float* g,
                                                const float* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::rowRgb(int y,
                                                const double* r,
                                                const double* g,
                                                const double* b)
{
    int rowOffset = y * _width;
    for (int x = 0, dataIdx = 0; x < _width; x++, dataIdx += _stride)
    {
        uint16_t tmp = ELS::PixUtils::convertRangeToHist(r[dataIdx]);
        uint8_t red = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(g[dataIdx]);
        uint8_t green = _lut[tmp];

        tmp = ELS::PixUtils::convertRangeToHist(b[dataIdx]);
        uint8_t blue = _lut[tmp];

        _qiData[rowOffset + x] = (0xff << 24) |
                                 (red << 16) |
                                 (green << 8) |
                                 (blue);
    }
}

void ImageFileListItem::ToQImageVisitor::done()
{
    _qi.reset(new QImage((const uchar*)_qiData.get(), _width, _height, QImage::Format_ARGB32));
}
