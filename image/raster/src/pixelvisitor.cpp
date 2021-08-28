#include "pixelvisitor.h"
#include "pixelvisitortypemismatch.h"

namespace ELS
{

    void PixelVisitor::rowGray(int y,
                               const int8_t* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 8-bit signed samples");
    }

    void PixelVisitor::rowGray(int y,
                               const int16_t* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 16-bit signed samples");
    }

    void PixelVisitor::rowGray(int y,
                               const int32_t* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 32-bit signed samples");
    }

    void PixelVisitor::rowGray(int y,
                               const uint8_t* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 8-bit unsigned samples");
    }

    void PixelVisitor::rowGray(int y,
                               const uint16_t* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 16-bit unsigned samples");
    }

    void PixelVisitor::rowGray(int y,
                               const uint32_t* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 32-bit unsigned samples");
    }

    void PixelVisitor::rowGray(int y,
                               const float* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 32-bit floating point samples");
    }

    void PixelVisitor::rowGray(int y,
                               const double* k)
    {
        (void)y;
        (void)k;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 64-bit floating point samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const int8_t* r,
                              const int8_t* g,
                              const int8_t* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 8-bit signed samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const int16_t* r,
                              const int16_t* g,
                              const int16_t* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 16-bit signed samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const int32_t* r,
                              const int32_t* g,
                              const int32_t* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 32-bit signed samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const uint8_t* r,
                              const uint8_t* g,
                              const uint8_t* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 8-bit unsigned samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const uint16_t* r,
                              const uint16_t* g,
                              const uint16_t* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 16-bit unsigned samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const uint32_t* r,
                              const uint32_t* g,
                              const uint32_t* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 32-bit unsigned samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const float* r,
                              const float* g,
                              const float* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 32-bit floating point samples");
    }

    void PixelVisitor::rowRgb(int y,
                              const double* r,
                              const double* g,
                              const double* b)
    {
        (void)y;
        (void)r;
        (void)g;
        (void)b;

        throw new PixelVisitorTypeMismatch("This PixelVisitor doesn't handle 64-bit floating point samples");
    }

}
