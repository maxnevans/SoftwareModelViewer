#include "pch.h"
#include "Rasterizer.h"
#include "Core.h"

namespace ModelViewer
{
    namespace Engine
    {
        Rasterizer::Rasterizer(Gdiplus::Bitmap& frame)
            :
            m_Frame(frame)
        {
        }

        void Rasterizer::drawPixel(int x, int y, Color color)
        {
            expect(x >= 0);
            expect((UINT)x < m_Frame.GetWidth());
            expect(y >= 0);
            expect((UINT)y < m_Frame.GetHeight());

            m_Frame.SetPixel(x, y, Gdiplus::Color{ 0xFF, color.r, color.g, color.b});
        }

        void Rasterizer::drawLine(int x1, int y1, int x2, int y2, Color color)
        {
            /*expect(x1 >= 0);
            expect((UINT)x1 < m_Frame.GetWidth());
            expect(y1 >= 0);
            expect((UINT)y1 < m_Frame.GetHeight());
            expect(x2 >= 0);
            expect((UINT)x2 < m_Frame.GetWidth());
            expect(y2 >= 0);
            expect((UINT)y2 < m_Frame.GetHeight());*/

            // DDA-line algorithm

            int xDistance = std::abs(x1 - x2);
            int yDistance = std::abs(y1 - y2);

            int numberIterations = xDistance > yDistance ? xDistance : yDistance;

            float x = static_cast<float>(x1);
            float y = static_cast<float>(y1);
            for (int i = 0; i < numberIterations; i++)
            {
                auto xGrowth = static_cast<float>(x2 - x1) / numberIterations;
                auto yGrowth = static_cast<float>(y2 - y1) / numberIterations;
                x += xGrowth;
                y += yGrowth;

                if (x < 0 || x >= m_Frame.GetWidth()
                    || y < 0 || y >= m_Frame.GetHeight())
                    continue;

                drawPixel(static_cast<int>(x), static_cast<int>(y), color);
            }
        }
    }
}

