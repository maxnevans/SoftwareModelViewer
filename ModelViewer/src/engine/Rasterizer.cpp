#include "pch.h"
#include "Rasterizer.h"
#include "Core.h"

namespace ModelViewer
{
    namespace Engine
    {

        constexpr void expectPoint(int x, int y, int screenWidth, int screenHeight)
        {
            expect(x >= 0);
            expect(x < screenWidth);
            expect(y >= 0);
            expect(y < screenHeight);
        }

        template<typename T>
        constexpr void expectVec2(Vec2<T> vec, int screenWidth, int screenHeight)
        {
            expect(vec[0] >= 0);
            expect(vec[0] < screenWidth);
            expect(vec[1] >= 0);
            expect(vec[1] < screenHeight);
        }

        Rasterizer::Rasterizer(int width, int height)
            :
            m_width(width),
            m_height(height),
            m_data(width * height),
            m_zBuffer(width * height)
        {
        }

        void Rasterizer::begin()
        {
            std::memset(m_data.data(), 0, m_data.size() * sizeof(unsigned));
            m_zBuffer.assign(m_zBuffer.size(), (std::numeric_limits<double>::max)());
        }

        void Rasterizer::end(Gdiplus::Graphics& gfx)
        {
            Gdiplus::Bitmap bitmap(m_width, m_height, m_width * STRIDE, PixelFormat32bppRGB, reinterpret_cast<BYTE*>(m_data.data()));
            gfx.DrawImage(&bitmap, 0, 0, m_width, m_height);
        }

        void Rasterizer::drawPixel(int x, int y, Color color)
        {
            expectPoint(x, y, m_width, m_height);

            m_data[y * m_width + x] = color.r << 16 | color.g << 8 | color.b;
        }

        void Rasterizer::drawPixel(int x, int y, double z, Color color)
        {
            if (z <= 0)
                return;

            if (m_zBuffer[y * m_width + x] <= z)
                return;

            m_zBuffer[y * m_width + x] = z;
            drawPixel(x, y, color);
        }

        void Rasterizer::drawLine(int x1, int y1, int x2, int y2, Color color)
        {
            expectPoint(x1, y1, m_width, m_height);
            expectPoint(x2, y2, m_width, m_height);

            // DDA-line algorithm

            int xDistance = std::abs(x1 - x2);
            int yDistance = std::abs(y1 - y2);

            int numberIterations = xDistance > yDistance ? xDistance : yDistance;

            float x = static_cast<float>(x1);
            float y = static_cast<float>(y1);
            for (int i = 0; i < numberIterations; i++)
            {
                drawPixel(static_cast<int>(x), static_cast<int>(y), color);

                auto xGrowth = static_cast<float>(x2 - x1) / numberIterations;
                auto yGrowth = static_cast<float>(y2 - y1) / numberIterations;
                x += xGrowth;
                y += yGrowth;
            }
            drawPixel(static_cast<int>(x), static_cast<int>(y), color);
        }

        void Rasterizer::drawLine(int x1, int y1, double z1, int x2, int y2, double z2, Color color)
        {
            if (z1 <= 0 && z2 <= 0)
                return;

            expectPoint(x1, y1, m_width, m_height);
            expectPoint(x2, y2, m_width, m_height);

            const int xDistance = std::abs(x1 - x2);
            const int yDistance = std::abs(y1 - y2);
            const auto zDistance = std::abs(z1 - z2);

            // Clip to Z = 0;

            if (z1 < 0)
            {
                const auto alphaZ = z1 / zDistance;
                x1 = static_cast<int>(x1 - alphaZ * xDistance);
                y1 = static_cast<int>(y1 - alphaZ * yDistance);
                z1 = 0;
            } else if (z2 < 0)
            {
                const auto betaZ = z2 / zDistance;
                x2 = static_cast<int>(x2 - betaZ * xDistance);
                y2 = static_cast<int>(y2 - betaZ * yDistance);
                z2 = 0;
            }          

            // DDA-line algorithm

            int numberIterations = xDistance > yDistance ? xDistance : yDistance;

            float x = static_cast<float>(x1);
            float y = static_cast<float>(y1);
            double z = z1;
            for (int i = 0; i < numberIterations; i++)
            {
                drawPixel(static_cast<int>(x), static_cast<int>(y), z, color);

                auto xGrowth = static_cast<float>(x2 - x1) / numberIterations;
                auto yGrowth = static_cast<float>(y2 - y1) / numberIterations;
                auto zGrowth = (z2 - z1) / numberIterations;
                x += xGrowth;
                y += yGrowth;
                z += zGrowth;
            }
            drawPixel(static_cast<int>(x), static_cast<int>(y), z, color);
        }

        void Rasterizer::drawHorizontalLine(Vec2<int>&& a, Vec2<int>&& b, Color&& color)
        {
            expectVec2(a, m_width, m_height);
            expectVec2(b, m_width, m_height);

            drawHorizontalLineUnsafe(std::forward<Vec2<int>>(a), std::forward<Vec2<int>>(b), std::forward<Color>(color));
        }

        void Rasterizer::drawHorizontalLine(Vec2<int>&& a, double zA, Vec2<int>&& b, double zB, Color&& color)
        {
            if (zA <= 0 && zB <= 0)
                return;

            expectVec2(a, m_width, m_height);
            expectVec2(b, m_width, m_height);

            drawHorizontalLineUnsafe(std::forward<Vec2<int>>(a), zA, std::forward<Vec2<int>>(b), zB, std::forward<Color>(color));
        }

        void Rasterizer::drawTriangle(Vec2<int> a, Vec2<int> b, Vec2<int> c, Color color)
        {
            // Assertion for vieport bounds
            expectVec2(a, m_width, m_height);
            expectVec2(b, m_width, m_height);
            expectVec2(c, m_width, m_height);

            // Don't care about 0 height triangle
            if (a[Y] == b[Y] && b[Y] == c[Y])
                return;

            // Sort by Y asc
            if (a[Y] > b[Y]) std::swap(a, b);
            if (b[Y] > c[Y]) std::swap(b, c);
            if (a[Y] > b[Y]) std::swap(a, b);

            const int totalHeight = c[Y] - a[Y];
            const auto alphaDistanceVec = c - a;

            const auto drawBetaPartTriangle = [this](const Vec2<int>& a, const Vec2<int>& b, const Vec2<int>& zeroPoint, 
                int totalHeight, const Vec2<int>& alphaDistanceVec, Color color)
            {
                const int segmentHeight = b[Y] - a[Y] + 1;
                const auto betaDistanceVec = b - a;

                for (int y = a[Y]; y <= b[Y]; y++)
                {
                    const double alpha = (static_cast<double>(y) - zeroPoint[Y]) / totalHeight;
                    const double beta = (static_cast<double>(y) - a[Y]) / segmentHeight; // Don't care zero division: always 1 or greater

                    double alphaX = zeroPoint[X] + static_cast<double>(alphaDistanceVec[X]) * alpha;
                    double betaX = a[X] + static_cast<double>(betaDistanceVec[X]) * beta;

                    if (alphaX > betaX) std::swap(alphaX, betaX);

                    drawHorizontalLineUnsafe(static_cast<int>(alphaX - 1), static_cast<int>(std::ceil(betaX + 2)), y, color);
                }
            };

            // Draw top beta part
            drawBetaPartTriangle(a, b, a, totalHeight, alphaDistanceVec, color);

            // Draw bottom beta part
            drawBetaPartTriangle(b, c, a, totalHeight, alphaDistanceVec, color);            
        }

        void Rasterizer::drawTriangle(Vec2<int> a, double zA, Vec2<int> b, double zB, Vec2<int> c, double zC, Color color)
        {
            if (zA <= 0 && zB <= 0 && zC <= 0)
                return;

            // Assertion for vieport bounds
            expectVec2(a, m_width, m_height);
            expectVec2(b, m_width, m_height);
            expectVec2(c, m_width, m_height);

            // Don't care about 0 height triangle
            if (a[Y] == b[Y] && b[Y] == c[Y])
                return;

            // Sort by Y asc
            if (a[Y] > b[Y])
            {
                std::swap(a, b);
                std::swap(zA, zB);
            }
            if (b[Y] > c[Y])
            {
                std::swap(b, c);
                std::swap(zB, zC);
            }
            if (a[Y] > b[Y])
            {
                std::swap(a, b);
                std::swap(zA, zB);
            }

            const int totalHeight = c[Y] - a[Y];
            const auto alphaDistanceVec = c - a;
            const auto alphaZDistance = zC - zA;

            const auto drawBetaPartTriangle = [this](const Vec2<int>& a, double zA, const Vec2<int>& b, double zB, 
                const Vec2<int>& zeroPoint, double zZeroPoint, double alphaZDistance, 
                int totalHeight, const Vec2<int>& alphaDistanceVec, Color color)
            {
                const int segmentHeight = b[Y] - a[Y] + 1;
                const auto betaDistanceVec = b - a;
                const auto betaZDistance = zB - zA;

                for (int y = a[Y]; y <= b[Y]; y++)
                {
                    const double alpha = (static_cast<double>(y) - zeroPoint[Y]) / totalHeight;
                    const double beta = (static_cast<double>(y) - a[Y]) / segmentHeight; // Don't care zero division: always 1 or greater

                    double alphaX = zeroPoint[X] + static_cast<double>(alphaDistanceVec[X]) * alpha;
                    double alphaZ = zZeroPoint + alphaZDistance * alpha;
                    double betaX = a[X] + static_cast<double>(betaDistanceVec[X]) * beta;
                    double betaZ = zA + betaZDistance * beta;

                    if (alphaX > betaX)
                    {
                        std::swap(alphaX, betaX);
                        std::swap(alphaZ, betaZ);
                    }

                    drawHorizontalLineUnsafe(static_cast<int>(alphaX - 1), alphaZ, static_cast<int>(std::ceil(betaX + 2)), betaZ, y, color);
                }
            };

            // Draw top beta part
            drawBetaPartTriangle(a, zA, b, zB,  a, zA, alphaZDistance, totalHeight, alphaDistanceVec, color);

            // Draw bottom beta part
            drawBetaPartTriangle(b, zB, c, zC, a, zA, alphaZDistance, totalHeight, alphaDistanceVec, color);
        }

        void Rasterizer::drawQuadrangle(Vec3<double> a, Vec3<double> b, Vec3<double> c, Vec3<double> d, Color color)
        {
            drawTriangle(a, a[Z], b, b[Z], c, c[Z], color);
            drawTriangle(a, a[Z], c, c[Z], d, d[Z], color);
        }

        void Rasterizer::drawHorizontalLineUnsafe(const Vec2<int>& a, const Vec2<int>& b, Color color)
        {
            for (int x = a[X]; x < b[X]; x++)
                drawPixel(x, a[Y], color);
        }

        void Rasterizer::drawHorizontalLineUnsafe(int minX, int maxX, int y, Color color)
        {
            for (int x = minX; x < maxX; x++)
                drawPixel(x, y, color);
        }
        void Rasterizer::drawHorizontalLineUnsafe(const Vec2<int>& a, double zA, const Vec2<int>& b, double zB, Color color)
        {
            double zGrowth = std::abs(zB - zA) / std::abs(b[X] - a[X]);
            double z = zA;

            for (int x = a[X]; x < b[X]; x++)
            {
                drawPixel(x, a[Y], z, color);
                z += zGrowth;
            }
        }

        void Rasterizer::drawHorizontalLineUnsafe(int minX, double zMinX, int maxX, double zMaxX, int y, Color color)
        {
            double zGrowth = std::abs(zMaxX - zMinX) / std::abs(maxX - minX);
            double z = zMinX;

            for (int x = minX; x < maxX; x++)
            {
                drawPixel(x, y, z, color);
                z += zGrowth;
            } 
        }
    }
}

