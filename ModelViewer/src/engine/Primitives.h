#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer::Engine::Primitives
{
    template<typename T, std::size_t CountVertices>
    using GenericPrimitive = std::array<std::reference_wrapper<const Vector4<T>>, CountVertices>;

    constexpr std::size_t LINE_VERTICES_COUNT = 2;
    template<typename T>
    using Line = GenericPrimitive<T, LINE_VERTICES_COUNT>;

    using IntLine = Line<int>;

    constexpr std::size_t TRIANGLE_VERTICES_COUNT = 3;
    template<typename T>
    using Triangle = GenericPrimitive<T, TRIANGLE_VERTICES_COUNT>;

    using IntTriangle = Triangle<int>;
    using FltTriangle = Triangle<double>;

    inline Vector3<double> calcTriangleNormal(const FltTriangle& triangle)
    {
        const auto a = triangle[0].get() - triangle[1].get();
        const auto b = triangle[0].get() - triangle[2].get();
        return static_cast<Vector3<double>>(a.crossProduct(b)).normalize();
    }

    inline bool isTriangleTowardsCamera(const Vector3<double>& cameraVector, 
        const FltTriangle& triangle)
    {
        return calcTriangleNormal(triangle).cos(cameraVector) < 0;
    }

    inline unsigned char calcPointRegionCode(const int left, const int top, const int right, 
        const int bottom, const double x, const double y)
    {
        unsigned char flags = 0;
        flags |= x < left;
        flags |= (x >= right) << 1;
        flags |= (y < top) << 2;
        flags |= (y >= bottom) << 3;

        return flags;
    }

    inline std::optional<std::pair<Vector4<int>, Vector4<int>>> clipLine(const int x, const  int y, 
        const int width, const int height, const std::pair<Vector4<int>, Vector4<int>>& line)
    {
        const int xWidth = x + width;
        const int yHeight = y + height;

        constexpr int X = 0;
        constexpr int Y = 1;

        auto v1 = static_cast<Vector4<double>>(line.first);
        auto v2 = static_cast<Vector4<double>>(line.second);

        unsigned char flags[2] = {
            calcPointRegionCode(x, y, xWidth, yHeight, v1[X], v1[Y]),
            calcPointRegionCode(x, y, xWidth, yHeight, v2[X], v2[Y])
        };

        constexpr auto INSIDE   = 0b0000;
        constexpr auto LEFT     = 0b0001;
        constexpr auto RIGHT    = 0b0010;
        constexpr auto BOTTOM   = 0b0100;
        constexpr auto TOP      = 0b1000;

        while (true) {
            if (!(flags[0] | flags[1]))
            {
                // bitwise OR is 0: both points inside window; trivially accept and exit loop
                return std::pair{ std::move(static_cast<Vector4<int>>(v1)), std::move(static_cast<Vector4<int>>(v2)) };
            }
            else if (flags[0] & flags[1])
            {
                // bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
                // or BOTTOM), so both must be outside window; exit loop (accept is false)
                return std::nullopt;
            }
            else 
            {
                // failed both tests, so calculate the line segment to clip
                // from an outside point to an intersection with clip edge
                double xNew = -1, yNew = -1;

                // At least one endpoint is outside the clip rectangle; pick it.
                unsigned char outcodeOut = flags[1] > flags[0] ? flags[1] : flags[0];

                // Now find the intersection point;
                // use formulas:
                //   slope = (y1 - y0) / (x1 - x0)
                //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
                //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
                // No need to worry about divide-by-zero because, in each case, the
                // outcode bit being tested guarantees the denominator is non-zero
                if (outcodeOut & TOP) {           // point is above the clip window
                    xNew = v1[X] + (v2[X] - v1[X]) * (yHeight - 1 - v1[Y]) / (v2[Y] - v1[Y]);
                    yNew = yHeight - 1;
                }
                else if (outcodeOut & BOTTOM) { // point is below the clip window
                    xNew = v1[X] + (v2[X] - v1[X]) * (y - v1[Y]) / (v2[Y] - v1[Y]);
                    yNew = y;
                }
                else if (outcodeOut & RIGHT) {  // point is to the right of clip window
                    yNew = v1[Y] + (v2[Y] - v1[Y]) * (xWidth - 1 - v1[X]) / (v2[X] - v1[X]);
                    xNew = xWidth - 1;
                }
                else if (outcodeOut & LEFT) {   // point is to the left of clip window
                    yNew = v1[Y] + (v2[Y] - v1[Y]) * (x - v1[X]) / (v2[X] - v1[X]);
                    xNew = x;
                }

                // Now we move outside point to intersection point to clip
                // and get ready for next pass.
                if (outcodeOut == flags[0]) {
                    v1[X] = xNew;
                    v1[Y] = yNew;
                    flags[0] = calcPointRegionCode(x, y, xWidth, yHeight, v1[X], v1[Y]);
                }
                else {
                    v2[X] = xNew;
                    v2[Y] = yNew;
                    flags[1] = calcPointRegionCode(x, y, xWidth, yHeight, v2[X], v2[Y]);
                }
            }
        }
    }
}