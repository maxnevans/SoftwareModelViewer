#pragma once
#include "pch.h"
#include "math/Vector.h"
#include "math/Matrix.h"
#include "engine/ObjectParser.h"
#include "engine/Color.h"
#include "engine/DiffuseMap.h"
#include "engine/NormalMap.h"
#include "engine/SpecularMap.h"

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
                void setDiffuseMap(DiffuseMap diffuseMap);
                void setNormalMap(NormalMap normalMap);
                void setSpecularMap(SpecularMap specularMap);
                const DiffuseMap& getDiffuseMap() const;
                const NormalMap& getNormalMap() const;
                const SpecularMap& getSpecularMap() const;

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
                    DiffuseMap diffuseMap;
                    NormalMap normalMap;
                    SpecularMap specularMap;
                } m_textures;
            };
        }
    }
}