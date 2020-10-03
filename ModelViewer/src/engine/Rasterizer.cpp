#include "pch.h"
#include "Rasterizer.h"
#include "Core.h"

namespace ModelViewer
{
    namespace Engine
    {
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
            expect(x >= 0);
            expect((UINT)x < m_width);
            expect(y >= 0);
            expect((UINT)y < m_height);

            m_data[y * m_width + x] = color.r << 16 | color.g << 8 | color.b;
        }

        void Rasterizer::drawLine(int x1, int y1, int x2, int y2, Color color)
        {
            expect(x1 >= 0);
            expect((UINT)x1 < m_width);
            expect(y1 >= 0);
            expect((UINT)y1 < m_height);
            expect(x2 >= 0);
            expect((UINT)x2 < m_width);
            expect(y2 >= 0);
            expect((UINT)y2 < m_height);

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
    }
}

