#ifndef FITSWIDGET_H
#define FITSWIDGET_H

#include <QWidget>
#include <QString>
#include <fitsio.h>

#include "fitsimage.h"

class FITSWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FITSWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    const ELS::FITSImage *getImage() const;

public slots:
    void setFile(const char *filename);

signals:
    void fileChanged(const char *filename);
    void fileFailed(const char *filename,
                    const char *errText);

protected:
    void paintEvent(QPaintEvent *event) override;

    QImage *convertImage() const;
    void convertU16MonoImage(QImage *qi,
                             int width,
                             int height,
                             const uint16_t *pixels) const;
    void convertFloatMonoImage(QImage *qi,
                               int width,
                               int height,
                               const float *pixels) const;
    void convertDoubleMonoImage(QImage *qi,
                                int width,
                                int height,
                                const double *pixels) const;
    void convertU16ColorImage(QImage *qi,
                              int width,
                              int height,
                              ELS::FITSImage::PixelFormat pf,
                              const uint16_t *pixels) const;
    void convertFloatColorImage(QImage *qi,
                                int width,
                                int height,
                                ELS::FITSImage::PixelFormat pf,
                                const float *pixels) const;
    void convertDoubleColorImage(QImage *qi,
                                 int width,
                                 int height,
                                 ELS::FITSImage::PixelFormat pf,
                                 const double *pixels) const;

private:
    QSizePolicy _sizePolicy;
    const char *_filename;
    ELS::FITSImage *_fits;
    QImage *_cacheImage;
};

#endif // FITSWIDGET_H
