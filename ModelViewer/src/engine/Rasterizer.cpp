#include "pch.h"
#include "Rasterizer.h"
#include "Core.h"
#include "engine/light/Phong.h"

namespace ModelViewer
{
    namespace Engine
    {
        bool checkPoint(int x, int y, int screenWidth, int screenHeight)
        {
            return x >= 0 && x < screenWidth && y >= 0 && y < screenHeight;
        }

        void expectPoint(int x, int y, int screenWidth, int screenHeight)
        {
            expect(checkPoint(x, y, screenWidth, screenHeight));
        }

        template<typename T>
        bool checkVec2(Vec2<T> vec, int screenWidth, int screenHeight)
        {
            return vec[0] >= 0 && vec[0] < screenWidth && vec[1] >= 0 && vec[1] < screenHeight;
        }

        template<typename T>
        void expectVec2(Vec2<T> vec, int screenWidth, int screenHeight)
        {
            expect(checkVec2(vec, screenWidth, screenHeight));
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
            if (!checkPoint(x, y, m_width, m_height))
                return;

            if (z <= 0)
                return;

            if (m_zBuffer[y * m_width + x] <= z)
                return;

            m_zBuffer[y * m_width + x] = z;
            drawPixel(x, y, color);
        }

        void Rasterizer::drawPixel(int x, int y, double z, Color color, const Vec3<double>& normal, const Vec3<double>& worldVertex)
        {
            if (!checkPoint(x, y, m_width, m_height))
                return;

            if (z <= 0)
                return;

            if (m_zBuffer[y * m_width + x] <= z)
                return;

            // Update z-buffer
            m_zBuffer[y * m_width + x] = z;

            // TODO: calculate light

            Light::Phong light({ { 99, 179, 219  }, 50 }, { Vector3<double>({ 0, 0, 1.0 }), { 145, 155, 237}, 50 });

            color = light(normal, worldVertex, Vec3<double>({5.0, 0.0, 0.0}), color);

            drawPixel(x, y, color);
        }

        void Rasterizer::drawPixel(int x, int y, double z, Color color, const Vec3<double>& normal, const Vec3<double>& worldVertex, double ks)
        {
            if (!checkPoint(x, y, m_width, m_height))
                return;

            if (z <= 0)
                return;

            if (m_zBuffer[y * m_width + x] <= z)
                return;

            // Update z-buffer
            m_zBuffer[y * m_width + x] = z;

            // TODO: calculate light

            Light::Phong light({ { 99, 179, 219  }, 50 }, { Vector3<double>({ 0, 0, 5.0 }), { 145, 155, 237}, 50 });

            color = light(normal, worldVertex, Vec3<double>({ 5.0, 0.0, 0.0 }), color, ks);

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
            if (a[Y] < 0 || a[Y] >= m_height)
                return;

            expectVec2(a, m_width, m_height);
            expectVec2(b, m_width, m_height);

            expect(a[X] <= b[X]);

            drawHorizontalLineUnsafe(std::forward<Vec2<int>>(a), std::forward<Vec2<int>>(b), std::forward<Color>(color));
        }

        void Rasterizer::drawHorizontalLine(Vec2<int>&& a, double zA, Vec2<int>&& b, double zB, Color&& color)
        {
            if (a[Y] < 0 || a[Y] >= m_height)
                return;

            if (zA <= 0 && zB <= 0)
                return;

            expect(a[X] <= b[X]);

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

        void Rasterizer::drawTriangle(Vec2<int> a, double zA, Vec3<double> aNormal, Vec3<double> aWorldVertex,
            Vec2<int> b, double zB, Vec3<double> bNormal, Vec3<double> bWorldVertex, 
            Vec2<int> c, double zC, Vec3<double> cNormal, Vec3<double> cWorldVertex, Color color)
        {
            if (zA <= 0 && zB <= 0 && zC <= 0)
                return;

            // Don't care about 0 height triangle
            if (a[Y] == b[Y] && b[Y] == c[Y])
                return;

            // Sort by Y asc: A min, B, C max
            if (a[Y] > b[Y])
            {
                std::swap(a, b);
                std::swap(zA, zB);
                std::swap(aNormal, bNormal);
                std::swap(aWorldVertex, bWorldVertex);
            }
            if (b[Y] > c[Y])
            {
                std::swap(b, c);
                std::swap(zB, zC);
                std::swap(bNormal, cNormal);
                std::swap(bWorldVertex, cWorldVertex);
            }
            if (a[Y] > b[Y])
            {
                std::swap(a, b);
                std::swap(zA, zB);
                std::swap(aNormal, bNormal);
                std::swap(aWorldVertex, bWorldVertex);
            }

            const int totalHeight = c[Y] - a[Y];

            const Vec2<int> alphaDistanceVec = c - a;
            const double alphaZDistance = zC - zA;
            const Vec3<double> alphaNormalDistance = cNormal - aNormal;
            const Vec3<double> alphaWorldVertexDistance = static_cast<Vec3<double>>(cWorldVertex - aWorldVertex);

            const auto drawBetaPartTriangle = [this](const Vec2<int>& a, double zA, const Vec3<double>& aNormal, const Vec3<double>& aWorldVertex,
                const Vec2<int>& b, double zB, const Vec3<double>& bNormal, const Vec3<double>& bWorldVertex,
                const Vec2<int>& zeroPoint, double zZeroPoint, const Vec3<double>& zeroPointNormal, const Vec3<double>& zeroPointWorldVertex,
                double alphaZDistance, const Vec3<double>& alphaNormalDistance, const Vec3<double>& alphaWorldVertexDistance,
                int totalHeight, const Vec2<int>& alphaDistanceVec, Color color)
            {
                const int segmentHeight = b[Y] - a[Y] + 1;
                const auto betaDistanceVec = b - a;
                const auto betaZDistance = zB - zA;
                const auto betaNormalDistance = bNormal - aNormal;
                const auto betaWorldVertexDistance = bWorldVertex - aWorldVertex;

                for (int y = a[Y]; y <= b[Y]; y++)
                {
                    const double alpha = (static_cast<double>(y) - zeroPoint[Y]) / totalHeight;
                    const double beta = (static_cast<double>(y) - a[Y]) / segmentHeight; // Don't care zero division: always 1 or greater

                    double alphaX = zeroPoint[X] + static_cast<double>(alphaDistanceVec[X]) * alpha;
                    double alphaZ = zZeroPoint + alphaZDistance * alpha;
                    Vec3<double> alphaNormal = zeroPointNormal + alphaNormalDistance * alpha;
                    Vec3<double> alphaWorldVertex = zeroPointWorldVertex + alphaWorldVertexDistance * alpha;

                    double betaX = a[X] + static_cast<double>(betaDistanceVec[X]) * beta;
                    double betaZ = zA + betaZDistance * beta;
                    Vec3<double> betaNormal = aNormal + betaNormalDistance * beta;
                    Vec3<double> betaWorldVertex = aWorldVertex + betaWorldVertexDistance * beta;

                    if (alphaX > betaX)
                    {
                        std::swap(alphaX, betaX);
                        std::swap(alphaZ, betaZ);
                        std::swap(alphaNormal, betaNormal);
                        std::swap(alphaWorldVertex, betaWorldVertex);
                    }

                    drawHorizontalLineUnsafe(static_cast<int>(alphaX - 1), alphaZ, alphaNormal, alphaWorldVertex,
                        static_cast<int>(std::ceil(betaX + 2)), betaZ, betaNormal, betaWorldVertex, y, color);
                }
            };

            // Draw top beta part
            drawBetaPartTriangle(a, zA, aNormal, aWorldVertex,
                b, zB, bNormal, bWorldVertex,
                a, zA, aNormal, aWorldVertex,
                alphaZDistance, alphaNormalDistance, alphaWorldVertexDistance,
                totalHeight, alphaDistanceVec, color);

            // Draw bottom beta part
            drawBetaPartTriangle(b, zB, bNormal, bWorldVertex,
                c, zC, cNormal, cWorldVertex,
                a, zA, aNormal, aWorldVertex,
                alphaZDistance, alphaNormalDistance, alphaWorldVertexDistance,
                totalHeight, alphaDistanceVec, color);
        }

        void Rasterizer::drawTriangle(Vec2<int> a, double zA, Vec3<double> aWorldVertex, Vec3<double> uvA,
            Vec2<int> b, double zB, Vec3<double> bWorldVertex, Vec3<double> uvB,
            Vec2<int> c, double zC, Vec3<double> cWorldVertex, Vec3<double> uvC,
            const DiffuseMap& diffuseMap, const NormalMap& normalMap, const SpecularMap& specularMap)
        {
            if (zA <= 0 && zB <= 0 && zC <= 0)
                return;

            // Don't care about 0 height triangle
            if (a[Y] == b[Y] && b[Y] == c[Y])
                return;

            // Sort by Y asc: A min, B, C max
            if (a[Y] > b[Y])
            {
                std::swap(a, b);
                std::swap(zA, zB);
                std::swap(aWorldVertex, bWorldVertex);
                std::swap(uvA, uvB);
            }
            if (b[Y] > c[Y])
            {
                std::swap(b, c);
                std::swap(zB, zC);
                std::swap(bWorldVertex, cWorldVertex);
                std::swap(uvB, uvC);
            }
            if (a[Y] > b[Y])
            {
                std::swap(a, b);
                std::swap(zA, zB);
                std::swap(aWorldVertex, bWorldVertex);
                std::swap(uvA, uvB);
            }

            const int totalHeight = c[Y] - a[Y];

            // Perspective correction for UV
            uvA /= zA;
            uvB /= zB;
            uvC /= zC;

            const double aUVCorrection = 1 / zA;
            const double bUVCorrection = 1 / zB;
            const double cUVCorrection = 1 / zC;

            const Vec2<int> alphaDistanceVec = c - a;
            const double alphaZDistance = zC - zA;
            const Vec3<double> alphaWorldVertexDistance = static_cast<Vec3<double>>(cWorldVertex - aWorldVertex);
            const Vec3<double> alphaUVDistance = uvC - uvA;
            const double alphaUVCorrectionDistance = cUVCorrection - aUVCorrection;

            const auto drawBetaPartTriangle = [this](const Vec2<int>& a, double zA, const Vec3<double>& aWorldVertex, const Vec3<double>& uvA, double aUVCorrection,
                const Vec2<int>& b, double zB, const Vec3<double>& bWorldVertex, const Vec3<double>& uvB, double bUVCorrection,
                const Vec2<int>& zeroPoint, double zZeroPoint, const Vec3<double>& zeroPointWorldVertex, const Vec3<double>& zeroPointUV, double zeroPointUVCorrection,
                double alphaZDistance, const Vec3<double>& alphaWorldVertexDistance, const Vec3<double>& alphaUVDistance, double alphaUVCorrectionDistance,
                int totalHeight, const Vec2<int>& alphaDistanceVec, 
                const DiffuseMap& diffuseMap, const NormalMap& normalMap, const SpecularMap& specularMap)
            {
                const int segmentHeight = b[Y] - a[Y] + 1;
                const auto betaDistanceVec = b - a;
                const auto betaZDistance = zB - zA;
                const auto betaUVDistance = uvB - uvA;
                const auto betaWorldVertexDistance = bWorldVertex - aWorldVertex;
                const auto betaUVCorrectionDistance = 1 / zB - 1 / zA;

                for (int y = a[Y]; y <= b[Y]; y++)
                {
                    const double alpha = (static_cast<double>(y) - zeroPoint[Y]) / totalHeight;
                    const double beta = (static_cast<double>(y) - a[Y]) / segmentHeight; // Don't care zero division: always 1 or greater

                    double alphaX = zeroPoint[X] + static_cast<double>(alphaDistanceVec[X]) * alpha;
                    double alphaZ = zZeroPoint + alphaZDistance * alpha;
                    Vec3<double> alphaUV = zeroPointUV + alphaUVDistance * alpha;
                    Vec3<double> alphaWorldVertex = zeroPointWorldVertex + alphaWorldVertexDistance * alpha;
                    auto alphaUVCorrection = zeroPointUVCorrection + alphaUVCorrectionDistance * alpha;

                    double betaX = a[X] + static_cast<double>(betaDistanceVec[X]) * beta;
                    double betaZ = zA + betaZDistance * beta;
                    Vec3<double> betaUV = uvA + betaUVDistance * beta;
                    Vec3<double> betaWorldVertex = aWorldVertex + betaWorldVertexDistance * beta;
                    auto betaUVCorrection = aUVCorrection + betaUVCorrectionDistance * beta;

                    if (alphaX > betaX)
                    {
                        std::swap(alphaX, betaX);
                        std::swap(alphaZ, betaZ);
                        std::swap(alphaUV, betaUV);
                        std::swap(alphaWorldVertex, betaWorldVertex);
                        std::swap(alphaUVCorrection, betaUVCorrection);
                    }

                    drawHorizontalLineUnsafe(static_cast<int>(alphaX - 1), alphaZ, alphaWorldVertex, alphaUV / alphaUVCorrection,
                        static_cast<int>(std::ceil(betaX + 2)), betaZ, betaWorldVertex, betaUV / betaUVCorrection,
                        y, diffuseMap, normalMap, specularMap);
                }
            };

            // Draw top beta part
            drawBetaPartTriangle(a, zA, aWorldVertex, uvA, aUVCorrection,
                b, zB, bWorldVertex, uvB, bUVCorrection,
                a, zA, aWorldVertex, uvA, aUVCorrection,
                alphaZDistance, alphaWorldVertexDistance, alphaUVDistance, alphaUVCorrectionDistance,
                totalHeight, alphaDistanceVec, 
                diffuseMap, normalMap, specularMap);

            // Draw bottom beta part
            drawBetaPartTriangle(b, zB, bWorldVertex, uvB, bUVCorrection,
                c, zC, cWorldVertex, uvC, cUVCorrection,
                a, zA, aWorldVertex, uvA, aUVCorrection,
                alphaZDistance, alphaWorldVertexDistance, alphaUVDistance, alphaUVCorrectionDistance,
                totalHeight, alphaDistanceVec,
                diffuseMap, normalMap, specularMap);
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
        void Rasterizer::drawHorizontalLineUnsafe(int minX, double zMinX, Vec3<double> minXNormal, Vec3<double> minXWorldVertex,
            int maxX, double zMaxX, Vec3<double> maxXNormal, Vec3<double> maxXWorldVertex, int y, Color color)
        {
            const double xDistance = std::abs(maxX - minX);

            const Vec3<double> normalGrowth = (maxXNormal - minXNormal) / xDistance;
            const Vec3<double> worldVertexGrowth = (maxXWorldVertex - minXWorldVertex) / xDistance;
            const double zGrowth = (zMaxX - zMinX) / xDistance;

            double z = zMinX;
            Vec3<double> normal = minXNormal;
            Vec3<double> worldVertex = minXWorldVertex;

            for (int x = minX; x < maxX; x++)
            {
                drawPixel(x, y, z, color, normal.normalize(), worldVertex);
                z += zGrowth;
                normal += normalGrowth;
            }
        }

        void Rasterizer::drawHorizontalLineUnsafe(int minX, double zMinX, Vec3<double> minXWorldVertex, Vec3<double> minXUV, 
            int maxX, double zMaxX, Vec3<double> maxXWorldVertex, Vec3<double> maxXUV, int y,
            const DiffuseMap& diffuseMap, const NormalMap& normalMap, const SpecularMap& specularMap)
        {
            const double xDistance = std::abs(maxX - minX);

            const Vec3<double> uvGrowth = (maxXUV - minXUV) / xDistance;
            const Vec3<double> worldVertexGrowth = (maxXWorldVertex - minXWorldVertex) / xDistance;
            const double zGrowth = (zMaxX - zMinX) / xDistance;

            double z = zMinX;
            Vec3<double> uv = minXUV;
            Vec3<double> worldVertex = minXWorldVertex;

            for (int x = minX; x < maxX; x++)
            {
                drawPixel(x, y, z, diffuseMap(uv[U], uv[V]), normalMap(uv[U], uv[V]), worldVertex, specularMap(uv[U], uv[V]));
                z += zGrowth;
                uv += uvGrowth;
            }
        }
    }
}

