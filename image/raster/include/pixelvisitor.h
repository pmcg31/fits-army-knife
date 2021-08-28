#pragma once

#include <inttypes.h>
#include "rastertypes.h"

namespace ELS
{

    class PixelVisitor
    {
    public:
        virtual void pixelFormat(ELS::PixelFormat pf) = 0;
        virtual void dimensions(int width, int height) = 0;
        virtual void rowInfo(int stride) = 0;

        virtual void rowGray(int y,
                             const int8_t *k);
        virtual void rowGray(int y,
                             const int16_t *k);
        virtual void rowGray(int y,
                             const int32_t *k);
        virtual void rowGray(int y,
                             const uint8_t *k);
        virtual void rowGray(int y,
                             const uint16_t *k);
        virtual void rowGray(int y,
                             const uint32_t *k);
        virtual void rowGray(int y,
                             const float *k);
        virtual void rowGray(int y,
                             const double *k);

        virtual void rowRgb(int y,
                            const int8_t *r,
                            const int8_t *g,
                            const int8_t *b);
        virtual void rowRgb(int y,
                            const int16_t *r,
                            const int16_t *g,
                            const int16_t *b);
        virtual void rowRgb(int y,
                            const int32_t *r,
                            const int32_t *g,
                            const int32_t *b);
        virtual void rowRgb(int y,
                            const uint8_t *r,
                            const uint8_t *g,
                            const uint8_t *b);
        virtual void rowRgb(int y,
                            const uint16_t *r,
                            const uint16_t *g,
                            const uint16_t *b);
        virtual void rowRgb(int y,
                            const uint32_t *r,
                            const uint32_t *g,
                            const uint32_t *b);
        virtual void rowRgb(int y,
                            const float *r,
                            const float *g,
                            const float *b);
        virtual void rowRgb(int y,
                            const double *r,
                            const double *g,
                            const double *b);

        virtual void done() = 0;
    };

}