#pragma once

#include <memory>
#include <QDataStream>
#include <QImage>
#include <QString>

#include "image.h"
#include "pixstfparms.h"

class ImageFileListItem
{
public:
    ImageFileListItem();
    ImageFileListItem(QString absolutePath,
                      ELS::Image::FileType fileType = ELS::Image::FT_UNKNOWN,
                      bool isValidated = false);
    ~ImageFileListItem();

    QString absolutePath() const;

    bool isValidated() const;
    bool isLoaded() const;
    bool showStretched() const;

    bool isColor() const;
    QString getMin() const;
    QString getMean() const;
    QString getMedian() const;
    QString getMax() const;
    int getNumHistogramPoints() const;
    std::shared_ptr<const uint32_t[]> getHistogram() const;

    std::shared_ptr<const QImage> getQImage() const;

    void setValidated(bool isValidated);
    void setShowStretched(bool showStretched);

    void load();

    void streamTo(QDataStream& out) const;
    void streamFrom(QDataStream& in);

    bool operator==(const ImageFileListItem& rhs) const;
    bool operator!=(const ImageFileListItem& rhs) const;
    bool operator<(const ImageFileListItem& rhs) const;
    bool operator>(const ImageFileListItem& rhs) const;
    bool operator<=(const ImageFileListItem& rhs) const;
    bool operator>=(const ImageFileListItem& rhs) const;

private:
    void calculateStatistics();
    void calculateLUTs();

private:
    class ToQImageVisitor : public ELS::PixelVisitor
    {
    public:
        ToQImageVisitor(ELS::PixSTFParms stfParms,
                        uint8_t* lut,
                        int lutPoints);
        ~ToQImageVisitor();

        std::shared_ptr<uint32_t[]> getImageData();
        std::shared_ptr<QImage> getImage();

    public:
        virtual void pixelFormat(ELS::PixelFormat pf) override;
        virtual void dimensions(int width, int height) override;
        virtual void rowInfo(int stride) override;

        virtual void rowGray(int y,
                             const int8_t* k) override;
        virtual void rowGray(int y,
                             const int16_t* k) override;
        virtual void rowGray(int y,
                             const int32_t* k) override;
        virtual void rowGray(int y,
                             const uint8_t* k) override;
        virtual void rowGray(int y,
                             const uint16_t* k) override;
        virtual void rowGray(int y,
                             const uint32_t* k) override;
        virtual void rowGray(int y,
                             const float* k) override;
        virtual void rowGray(int y,
                             const double* k) override;

        virtual void rowRgb(int y,
                            const int8_t* r,
                            const int8_t* g,
                            const int8_t* b) override;
        virtual void rowRgb(int y,
                            const int16_t* r,
                            const int16_t* g,
                            const int16_t* b) override;
        virtual void rowRgb(int y,
                            const int32_t* r,
                            const int32_t* g,
                            const int32_t* b) override;
        virtual void rowRgb(int y,
                            const uint8_t* r,
                            const uint8_t* g,
                            const uint8_t* b) override;
        virtual void rowRgb(int y,
                            const uint16_t* r,
                            const uint16_t* g,
                            const uint16_t* b) override;
        virtual void rowRgb(int y,
                            const uint32_t* r,
                            const uint32_t* g,
                            const uint32_t* b) override;
        virtual void rowRgb(int y,
                            const float* r,
                            const float* g,
                            const float* b) override;
        virtual void rowRgb(int y,
                            const double* r,
                            const double* g,
                            const double* b) override;

        virtual void done() override;

    private:
        int _width;
        int _height;
        int _pixCount;
        std::shared_ptr<uint32_t[]> _qiData;
        int _stride;
        ELS::PixSTFParms _stfParms;
        std::shared_ptr<QImage> _qi;
        uint8_t* _lut;
        int _lutPoints;
    };

private:
    QString _absolutePath;
    ELS::Image::FileType _fileType;
    bool _isValidated;
    bool _isLoaded;
    bool _showStretched;

    std::shared_ptr<ELS::Image> _image;

    ELS::PixSTFParms _stfParms;
    QString _min;
    QString _mean;
    QString _median;
    QString _max;
    int _numHistogramPoints;
    std::shared_ptr<uint32_t[]> _histogram;

    uint8_t* _stfLUT;
    uint8_t* _identityLUT;
    uint8_t* _lutInUse;

    std::shared_ptr<uint32_t[]> _qiData;
    std::shared_ptr<QImage> _qi;
};

inline QDataStream& operator<<(QDataStream& out, const ImageFileListItem& item)
{
    item.streamTo(out);
    return out;
}

inline QDataStream& operator>>(QDataStream& in, ImageFileListItem& item)
{
    item.streamFrom(in);
    return in;
}