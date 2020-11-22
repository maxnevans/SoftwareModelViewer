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
            m_data(width * height, 0)
        {
        }

        void Rasterizer::begin()
        {
            std::memset(m_data.data(), 0, m_data.size() * 4);
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

        void Rasterizer::drawHorizonalLine(Vec2<int>&& a, Vec2<int>&& b, Color&& color)
        {
            expectVec2(a, m_width, m_height);
            expectVec2(b, m_width, m_height);

            drawHorizontalLineUnsafe(std::forward<Vec2<int>>(a), std::forward<Vec2<int>>(b), std::forward<Color>(color));
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
    }
}

