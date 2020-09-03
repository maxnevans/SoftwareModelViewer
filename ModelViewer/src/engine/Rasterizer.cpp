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
    }
}

