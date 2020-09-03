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

        private:
            Gdiplus::Bitmap& m_Frame;
        };
    }
}