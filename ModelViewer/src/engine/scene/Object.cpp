#include "pch.h"
#include "Object.h"
#include "engine/WorldMatrix.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            Object::Object(std::vector<Vec4<double>> vertices, std::vector<Vec3<double>> normals, 
                std::vector<Vec3<double>> textureCoords, std::vector<Index> indices, 
                std::vector<Color> colors, ColorType colorType)
                :
                m_Vertices(vertices),
                m_textureVertices(textureCoords),
                m_normals(normals),
                m_Indices(indices),
                m_colors(colors),
                m_colorType(colorType),
                m_TranslateVector({0,0,0}),
                m_RotateVector({0,0,0}),
                m_ScaleVector({1,1,1}),
                m_CacheModelMatrix(createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector)),
                m_CacheNormalModelMatrix(m_CacheModelMatrix.inverse().transpose())
            {
            }

            const std::vector<Vector4<double>>& Object::getVertices() const
            {
                return m_Vertices;
            }

            const std::vector<Vec3<double>>& Object::getTextureVertices() const
            {
                return m_textureVertices;
            }

            const std::vector<Vec3<double>>& Object::getNormals() const
            {
                return m_normals;
            }

            const std::vector<Index>& Object::getIndices() const
            {
                return m_Indices;
            }

            const std::vector<Color>& Object::getColors() const
            {
                return m_colors;
            }

            void Object::setColor(Color color)
            {
                m_colorType = ColorType::SOLID;
                m_colors.resize(1);
                m_colors[0] = color;
            }

            const Matrix4<double>& Object::getMatrix() const
            {
                return m_CacheModelMatrix;
            }

            const Matrix4<double>& Object::getNormalMatrix() const
            {
                return m_CacheNormalModelMatrix;
            }

            void Object::scaleX(double amount)
            {
                m_ScaleVector[0] = amount;
                updateCachedModelMatrices();
            }

            void Object::scaleY(double amount)
            {
                m_ScaleVector[1] = amount;
                updateCachedModelMatrices();
            }

            void Object::scaleZ(double amount)
            {
                m_ScaleVector[2] = amount;
                updateCachedModelMatrices();
            }

            void Object::scale(Vector4<double> amount)
            {
                m_ScaleVector = amount;
                updateCachedModelMatrices();
            }

            void Object::translateX(double amount)
            {
                m_TranslateVector[0] = amount;
                updateCachedModelMatrices();
            }

            void Object::translateY(double amount)
            {
                m_TranslateVector[1] = amount;
                updateCachedModelMatrices();
            }

            void Object::translateZ(double amount)
            {
                m_TranslateVector[2] = amount;
                updateCachedModelMatrices();
            }

            void Object::translate(Vector4<double> amount)
            {
                m_TranslateVector = amount;
                updateCachedModelMatrices();
            }

            void Object::rotateX(double amount)
            {
                m_RotateVector[0] = amount;
                updateCachedModelMatrices();
            }

            void Object::rotateY(double amount)
            {
                m_RotateVector[1] = amount;
                updateCachedModelMatrices();
            }

            void Object::rotateZ(double amount)
            {
                m_RotateVector[2] = amount;
                updateCachedModelMatrices();
            }

            void Object::rotate(Vector4<double> amount)
            {
                m_RotateVector = amount;
                updateCachedModelMatrices();
            }

            void Object::setDiffuseMap(std::vector<ColorChannel> data, std::size_t width, std::size_t height)
            {
                m_textures.diffuseMap = {};
                m_textures.diffuseMap.rawData = std::move(data);
                m_textures.diffuseMap.width = width;
                m_textures.diffuseMap.height = height;
                
            }

            void Object::setNormalMap(std::vector<ColorChannel> data, std::size_t width, std::size_t height)
            {
                m_textures.normalMap = {};
                m_textures.normalMap.rawData = std::move(data);
                m_textures.normalMap.width = width;
                m_textures.normalMap.height = height;
            }

            void Object::setSpecularMap(std::vector<ColorChannel> data, std::size_t width, std::size_t height)
            {
                m_textures.specularMap = {};
                m_textures.specularMap.rawData = std::move(data);
                m_textures.specularMap.width = width;
                m_textures.specularMap.height = height;
            }

            const Object::Texture& Object::getDiffuseMap() const
            {
                return m_textures.diffuseMap;
            }

            const Object::Texture& Object::getNormalMap() const
            {
                return m_textures.normalMap;
            }

            const Object::Texture& Object::getSpecularMap() const
            {
                return m_textures.specularMap;
            }

            Matrix4<double> Object::createModelMatrix(const Vector4<double>& translate, 
                const Vector4<double>& rotate, const Vector4<double>& scale) const
            {
                auto ret = createTranslateMatrix(translate);

                if (rotate[2] != 0)
                    ret *= createRotateZMatrix(rotate[2]);

                if (rotate[1] != 0)
                    ret *= createRotateYMatrix(rotate[1]);

                if (rotate[0] != 0)
                    ret *= createRotateXMatrix(rotate[0]);

                ret *= createScaleMatrix(scale);

                return ret;
            }

            void Object::updateCachedModelMatrices()
            {
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
                m_CacheNormalModelMatrix = m_CacheModelMatrix.inverse().transpose();
            }
        }
    }
}