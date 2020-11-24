#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer
{
    namespace Engine
    {
        struct Color
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;

            static constexpr unsigned char MAX = 0xFF;
            static constexpr unsigned char MIN = 0;
        };

        class Rasterizer
        {
        public:
            Rasterizer(int width, int height);
            void begin();
            void end(Gdiplus::Graphics& gfx);
            void drawPixel(int x, int y, Color color);
            void drawPixel(int x, int y, double z, Color color);
            void drawLine(int x1, int y1, int x2, int y2, Color color);
            void drawLine(int x1, int y1, double z1, int x2, int y2, double z2, Color color);
            void drawHorizontalLine(Vec2<int>&& a, Vec2<int>&& b, Color&& color);
            void drawHorizontalLine(Vec2<int>&& a, double zA, Vec2<int>&& b, double zB, Color&& color);
            void drawTriangle(Vec2<int> a, Vec2<int> b, Vec2<int> c, Color color);
            void drawTriangle(Vec2<int> a, double zA, Vec2<int> b, double zB, Vec2<int> c, double zC, Color color);
            void drawQuadrangle(Vec3<double> a, Vec3<double> b, Vec3<double> c, Vec3<double> d, Color color);
            inline UINT getWidth() const
            {
                return m_width;
            }
            inline UINT getHeight() const
            {
                return m_height;
            }

        private:
            void drawHorizontalLineUnsafe(const Vec2<int>& a, const Vec2<int>& b, Color color);
            void drawHorizontalLineUnsafe(int minX, int maxX, int y, Color color);
            void drawHorizontalLineUnsafe(const Vec2<int>& a, double zA, const Vec2<int>& b, double zB, Color color);
            void drawHorizontalLineUnsafe(int minX, double zMinX, int maxX, double zMaxX, int y, Color color);

        private:
            static constexpr int STRIDE = 4;

            int m_width;
            int m_height;
            std::vector<unsigned> m_data;
            std::vector<double> m_zBuffer;
        };
    }
}