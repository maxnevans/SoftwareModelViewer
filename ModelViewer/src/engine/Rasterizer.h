#pragma once
#include "pch.h"

namespace ModelViewer
{
    namespace Engine
    {
        struct Color
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };

        class Rasterizer
        {
        public:
            Rasterizer(Gdiplus::Bitmap& frame);
            void drawPixel(int x, int y, Color color);
            void drawLine(int x1, int y1, int x2, int y2, Color color);
            inline UINT getWidth() const
            {
                return m_Frame.GetWidth();
            }
            inline UINT getHeight() const
            {
                return m_Frame.GetHeight();
            }

        private:
            Gdiplus::Bitmap& m_Frame;
        };
    }
}