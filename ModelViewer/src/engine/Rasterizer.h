#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer
{
    namespace Engine
    {
        using ColorChannel = unsigned char;
        struct Color
        {
            ColorChannel r;
            ColorChannel g;
            ColorChannel b;

            static constexpr ColorChannel MAX = 0xFF;
            static constexpr ColorChannel MIN = 0;

            template<typename T>
            operator Vec3<T>() const
            {
                return Vec3<T>({ static_cast<T>(r), static_cast<T>(g), static_cast<T>(b) });
            }

            template<typename T>
            operator Vec4<T>() const
            {
                return Vec4<T>({ static_cast<T>(r), static_cast<T>(g), static_cast<T>(b), static_cast<T>(MAX) });
            }

            static inline ColorChannel boundColorChannel(double newColorChannel)
            {
                return static_cast<unsigned char>((std::max)((std::min)(newColorChannel, static_cast<double>(Color::MAX)),
                    static_cast<double>(Color::MIN)));
            }
        };

        class Rasterizer
        {
        public:
            Rasterizer(int width, int height);
            void begin();
            void end(Gdiplus::Graphics& gfx);
            void drawPixel(int x, int y, Color color);
            void drawPixel(int x, int y, double z, Color color);
            void drawPixel(int x, int y, double z, Color color, const Vec3<double>& normal, const Vec3<double>& worldVertex);
            void drawLine(int x1, int y1, int x2, int y2, Color color);
            void drawLine(int x1, int y1, double z1, int x2, int y2, double z2, Color color);
            void drawHorizontalLine(Vec2<int>&& a, Vec2<int>&& b, Color&& color);
            void drawHorizontalLine(Vec2<int>&& a, double zA, Vec2<int>&& b, double zB, Color&& color);
            void drawTriangle(Vec2<int> a, Vec2<int> b, Vec2<int> c, Color color);
            void drawTriangle(Vec2<int> a, double zA, Vec2<int> b, double zB, Vec2<int> c, double zC, Color color);
            void drawTriangle(Vec2<int> a, double zA, Vec3<double> aNormal, Vec3<double> aWorldVertex,
                Vec2<int> b, double zB, Vec3<double> bNormal, Vec3<double> bWorldVertex,
                Vec2<int> c, double zC, Vec3<double> cNormal, Vec3<double> cWorldVertex,
                Color color);
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
            void drawHorizontalLineUnsafe(int minX, double zMinX, Vec3<double> minXNormal, Vec3<double> minXWorldVertex,
                int maxX, double zMaxX, Vec3<double> maxXNormal, Vec3<double> maxXWorldVertex, int y, Color color);

        private:
            static constexpr int STRIDE = 4;

            int m_width;
            int m_height;
            std::vector<unsigned> m_data;
            std::vector<double> m_zBuffer;
        };
    }
}