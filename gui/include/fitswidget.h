#ifndef FITSWIDGET_H
#define FITSWIDGET_H

#include <QWidget>
#include <QWheelEvent>
#include <QString>
#include <fitsio.h>

#include "fits.h"
#include "fitsimage.h"

class FITSWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FITSWidget(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    const ELS::FITSImage *getImage() const;

    const char *getFilename() const;
    float getZoom() const;

public slots:
    void setFile(const char *filename);
    void setZoom(float zoom);

signals:
    void fileChanged(const char *filename);
    void fileFailed(const char *filename,
                    const char *errText);
    void zoomChanged(float zoom);
    void actualZoomChanged(float zoom);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

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
                              ELS::FITS::PixelFormat pf,
                              const uint16_t *pixels) const;
    void convertFloatColorImage(QImage *qi,
                                int width,
                                int height,
                                ELS::FITS::PixelFormat pf,
                                const float *pixels) const;
    void convertDoubleColorImage(QImage *qi,
                                 int width,
                                 int height,
                                 ELS::FITS::PixelFormat pf,
                                 const double *pixels) const;

protected:
    enum ZoomAdjustStrategy
    {
        ZAS_CLOSEST,
        ZAS_HIGHER,
        ZAS_LOWER
    };

    void _internalSetZoom(float zoom);

    static float adjustZoom(float desiredZoom,
                            ZoomAdjustStrategy strategy = ZAS_CLOSEST);

private:
    QSizePolicy _sizePolicy;
    const char *_filename;
    ELS::FITSImage *_fits;
    QImage *_cacheImage;
    float _zoom;
    float _actualZoom;

private:
    static const float g_validZooms[];
};

#endif // FITSWIDGET_H
