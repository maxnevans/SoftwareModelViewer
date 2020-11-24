#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer::Engine::Primitives
{
    // Generic stuff
    // ==========================================================

    template<typename T, std::size_t CountVertices>
    using GenericPrimitiveRef = std::array<std::reference_wrapper<const Vector4<T>>, CountVertices>;

    template<typename T, std::size_t CountVertices>
    using GenericPrimitive = std::array<Vector4<T>, CountVertices>;

    // Line
    // ==========================================================

    constexpr std::size_t LINE_VERTICES_COUNT = 2;

    template<typename T>
    using LineRef = GenericPrimitiveRef<T, LINE_VERTICES_COUNT>;

    template<typename T>
    using Line = GenericPrimitive<T, LINE_VERTICES_COUNT>;

    using IntLineRef = LineRef<int>;
    using FltLineRef = LineRef<double>;
    using IntLine = Line<int>;
    using FltLine = Line<double>;

    // Triangle
    // =========================================================

    constexpr std::size_t TRIANGLE_VERTICES_COUNT = 3;

    template<typename T>
    using TriangleRef = GenericPrimitiveRef<T, TRIANGLE_VERTICES_COUNT>;

    template<typename T>
    using Triangle = GenericPrimitive<T, TRIANGLE_VERTICES_COUNT>;

    using IntTriangleRef = TriangleRef<int>;
    using FltTriangleRef = TriangleRef<double>;
    using IntTriangle = Triangle<int>;
    using FltTriangle = Triangle<double>;

    // Quadrangle
    // =========================================================

    constexpr std::size_t QUADRANGLE_VERTICES_COUNT = 4;

    template<typename T>
    using QuadrangleRef = GenericPrimitiveRef<T, QUADRANGLE_VERTICES_COUNT>;

    template<typename T>
    using Quadrangle = GenericPrimitive<T, QUADRANGLE_VERTICES_COUNT>;

    using IntQuadrangleRef = QuadrangleRef<int>;
    using FltQuadrangleRef = QuadrangleRef<double>;
    using IntQuadrangle = Quadrangle<int>;
    using FltQuadrangle = Quadrangle<double>;

    // Pentagon
    // =========================================================

    constexpr std::size_t PENTAGON_VERTICES_COUNT = 5;

    template<typename T>
    using PentagonRef = GenericPrimitiveRef<T, PENTAGON_VERTICES_COUNT>;

    template<typename T>
    using Pentagon = GenericPrimitive<T, PENTAGON_VERTICES_COUNT>;

    using IntPentagonRef = PentagonRef<int>;
    using FltPentagonRef = PentagonRef<double>;
    using IntPentagon = Pentagon<int>;
    using FltPentagon = Pentagon<double>;

    // Hexagon
    // =========================================================

    constexpr std::size_t HEXAGON_VERTICES_COUNT = 6;

    template<typename T>
    using HexagonRef = GenericPrimitiveRef<T, HEXAGON_VERTICES_COUNT>;

    template<typename T>
    using Hexagon = GenericPrimitive<T, HEXAGON_VERTICES_COUNT>;

    using IntHexagonRef = HexagonRef<int>;
    using FltHexagonRef = HexagonRef<double>;
    using IntHexagon = Hexagon<int>;
    using FltHexagon = Hexagon<double>;

    // Heptagon
    // =========================================================

    constexpr std::size_t HEPTAGON_VERTICES_COUNT = 7;

    template<typename T>
    using HeptagonRef = GenericPrimitiveRef<T, HEPTAGON_VERTICES_COUNT>;

    template<typename T>
    using Heptagon = GenericPrimitive<T, HEPTAGON_VERTICES_COUNT>;

    using IntHeptagonRef = HeptagonRef<int>;
    using FltHeptagonRef = HeptagonRef<double>;
    using IntHeptagon = Heptagon<int>;
    using FltHeptagon = Heptagon<double>;

    // Octagon
    // =========================================================

    constexpr std::size_t OCTAGON_VERTICES_COUNT = 8;

    template<typename T>
    using OctagonRef = GenericPrimitiveRef<T, OCTAGON_VERTICES_COUNT>;

    template<typename T>
    using Octagon = GenericPrimitive<T, OCTAGON_VERTICES_COUNT>;

    using IntOctagonRef = OctagonRef<int>;
    using FltOctagonRef = OctagonRef<double>;
    using IntOctagon = Octagon<int>;
    using FltOctagon = Octagon<double>;

    // Functions
    // ==========================================================

    inline Vector3<double> calcTriangleNormal(const FltTriangleRef& triangle)
    {
        const auto a = triangle[0].get() - triangle[1].get();
        const auto b = triangle[0].get() - triangle[2].get();
        return static_cast<Vector3<double>>(a.crossProduct(b)).normalize();
    }

    inline bool isTriangleTowardsCamera(const Vector3<double>& cameraVector, 
        const FltTriangleRef& triangle)
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

    using TriangleClipVariant = std::variant<FltTriangle, FltQuadrangle, FltPentagon, 
        FltHexagon, FltHeptagon, FltOctagon>;

    inline std::optional<TriangleClipVariant> clipTriangle(const int x, const  int y,
        const int width, const int height, FltTriangleRef triangle)
    {
        // Triangle is behind camera
        if (triangle[0].get()[Z] <= 0
            && triangle[1].get()[Z] <= 0
            && triangle[2].get()[Z] <= 0)
            return std::nullopt;

        TriangleClipVariant output = {};

        // Clip vertices behind camera
        if (triangle[0].get()[Z] < 0
            || triangle[1].get()[Z] < 0
            || triangle[2].get()[Z] < 0)
        {

            const auto clipZ = [](const Vec4<double>& unknownVertexA, 
                const Vec4<double>& unknownVertexB, const Vec4<double>& negativeVertex) -> TriangleClipVariant
            {
                const auto clip2Sides = [](const Vec4<double>& vertexToClip, const Vec4<double>& a, const Vec4<double>& b)
                {
                    const auto zDistanceA = vertexToClip[Z] - a[Z];
                    const auto zDistanceB = vertexToClip[Z] - b[Z];
                    const auto zFactorA = a[Z] / zDistanceA;
                    const auto zFactorB = b[Z] / zDistanceB;

                    Vec4<double> newA = a;
                    Vec4<double> newB = b;

                    newA[Z] = 0;
                    newA[X] -= (vertexToClip[X] - a[X]) * zFactorA;
                    newA[Y] -= (vertexToClip[Y] - a[Y]) * zFactorA;

                    newB[Z] = 0;
                    newB[X] -= (vertexToClip[X] - b[X]) * zFactorB;
                    newB[Y] -= (vertexToClip[Y] - b[Y]) * zFactorB;

                    // Now we got triangle with new 2 vertices
                    return std::pair{ newA, newB };
                };

                // B is the only positive Z
                if (unknownVertexA[Z] < 0)
                {
                    auto [newA, newC] = clip2Sides(unknownVertexB, unknownVertexA, negativeVertex);
                    return FltTriangle{ newA, unknownVertexB, newC };
                }
                // A is the only positive Z
                else if (unknownVertexB[Z] < 0)
                {
                    auto [newB, newC] = clip2Sides(unknownVertexA, unknownVertexB, negativeVertex);
                    return FltTriangle{ unknownVertexA, newB, newC };
                }
                // A and B are positive Z
                else
                {
                    auto [newA, newB] = clip2Sides(negativeVertex, unknownVertexA, unknownVertexB);
                    return FltQuadrangle{ unknownVertexA, newA, newB, unknownVertexB};
                }
            };

            if (triangle[0].get()[Z] < 0)
            {
                output = clipZ(triangle[1].get(), triangle[2].get(), triangle[0].get());
            }
            else if (triangle[1].get()[Z] < 0)
            {
                output = clipZ(triangle[0].get(), triangle[2].get(), triangle[1].get());
            }
            else
            {
                output = clipZ(triangle[0].get(), triangle[1].get(), triangle[2].get());
            }
        }

        // Clipping sides out of screen        

        // Bool in pair: have line been clipped off?
        std::array<std::pair<std::optional<FltLine>, bool>, OCTAGON_VERTICES_COUNT> sidesBuffer = {};
        const int countVertices = std::get_if<FltTriangle>(&output) ? 3 : 4;

        if (countVertices == 3)
        {
            auto&& t = std::get<FltTriangle>(output);
            for (int i = 0; i < 3; i++)
                sidesBuffer[i * 2] = { FltLine{ t[i], t[(i + 1) % 3] }, false };
        }
        else
        {
            auto&& q = std::get<FltQuadrangle>(output);
            for (int i = 0; i < 4; i++)
                sidesBuffer[i * 2] = { FltLine{ q[i], q[(i + 1) % 4] }, false };
        }

        // Clip each side
        for (int i = 0; i < countVertices; i++)
        {
            auto clippedLine = clipLine(x, y, width, height, { (*sidesBuffer[i * 2].first)[0], (*sidesBuffer[i * 2].first)[1] });
            if (clippedLine)
            {
                sidesBuffer[i * 2] = { FltLine{ clippedLine->first, clippedLine->second }, false };
            }
            else
            {
                sidesBuffer[i * 2] = { std::nullopt, true };
            }
        }

        // Returns 0 if should go lower direction 1 other
        const auto calcContinueDirection = []()
        {
            // TODO
        };

        // Connect sides
        // TODO

        return std::nullopt;
    }
}