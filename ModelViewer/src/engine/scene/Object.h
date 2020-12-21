#pragma once
#include "pch.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "engine/ObjectParser.h"
#include "engine/Rasterizer.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            class Object
            {
            public:
                enum class ColorType
                {
                    SOLID,
                    PER_VERTEX,
                    PER_POLYGON,
                    PER_EDGE
                };

                struct Texture
                {
                    std::vector<ColorChannel> rawData;
                    std::size_t width;
                    std::size_t height;

                    inline ColorChannel operator()(double u, double v)
                    {
                        expect(u >= 0 && u <= 1);
                        expect(v >= 0 && v <= 1);
                        expect(width * height == rawData.size());

                        return rawData[static_cast<std::size_t>(v * height * width + u * width)];
                    }
                };

            public:
                Object(std::vector<Vector4<double>> vertices, std::vector<Vec3<double>> textureCoords, 
                    std::vector<Vec3<double>> normals, std::vector<Index> indices, 
                    std::vector<Color> colors = { {0xFF, 0xFF, 0xFF} }, ColorType colorType = ColorType::SOLID);
                const std::vector<Vector4<double>>& getVertices() const;
                const std::vector<Vec3<double>>& getTextureVertices() const;
                const std::vector<Vec3<double>>& getNormals() const;
                const std::vector<Index>& getIndices() const;
                const std::vector<Color>& getColors() const;
                void setColor(Color color);
                const Matrix4<double>& getMatrix() const;
                const Matrix4<double>& getNormalMatrix() const;
                void scaleX(double amount);
                void scaleY(double amount);
                void scaleZ(double amount);
                void scale(Vector4<double> amount);
                void translateX(double amount);
                void translateY(double amount);
                void translateZ(double amount);
                void translate(Vector4<double> amount);
                void rotateX(double amount);
                void rotateY(double amount);
                void rotateZ(double amount);
                void rotate(Vector4<double> amount);
                void setDiffuseMap(std::vector<ColorChannel> data, std::size_t width, std::size_t height);
                void setNormalMap(std::vector<ColorChannel> data, std::size_t width, std::size_t height);
                void setSpecularMap(std::vector<ColorChannel> data, std::size_t width, std::size_t height);
                const Texture& getDiffuseMap() const;
                const Texture& getNormalMap() const ;
                const Texture& getSpecularMap() const;

            private:
                Matrix4<double> createModelMatrix(const Vector4<double>& translate, 
                    const Vector4<double>& rotate, const Vector4<double>& scale) const;
                void updateCachedModelMatrices();

            private:
                std::vector<Vector4<double>> m_Vertices;
                std::vector<Vec3<double>> m_textureVertices;
                std::vector<Vec3<double>> m_normals;
                std::vector<Index> m_Indices;
                std::vector<Color> m_colors;
                ColorType m_colorType;

                Vector4<double> m_TranslateVector;
                Vector4<double> m_RotateVector;
                Vector4<double> m_ScaleVector;
                Matrix4<double> m_CacheModelMatrix;
                Matrix4<double> m_CacheNormalModelMatrix;

                struct {
                    Texture diffuseMap;
                    Texture normalMap;
                    Texture specularMap;
                } m_textures;
            };
        }
    }
}