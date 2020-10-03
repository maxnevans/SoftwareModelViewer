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
            Rasterizer(int width, int height);
            void begin();
            void end(Gdiplus::Graphics& gfx);
            void drawPixel(int x, int y, Color color);
            void drawLine(int x1, int y1, int x2, int y2, Color color);
            inline UINT getWidth() const
            {
                return m_width;
            }
            inline UINT getHeight() const
            {
                return m_height;
            }

        private:
            static constexpr int STRIDE = 4;

            int m_width;
            int m_height;
            std::vector<unsigned> m_data;
        };
    }
}