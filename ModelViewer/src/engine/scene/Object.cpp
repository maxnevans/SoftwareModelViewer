#include "pch.h"
#include "Object.h"
#include "engine/WorldMatrix.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            Object::Object(std::vector<Vec4<double>> vertices, std::vector<Vec3<double>> normals, std::vector<Index> indices)
                :
                m_Vertices(vertices),
                m_normals(normals),
                m_Indices(indices),
                m_TranslateVector({0,0,0}),
                m_RotateVector({0,0,0}),
                m_ScaleVector({1,1,1}),
                m_CacheModelMatrix(createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector))
            {
            }

            const std::vector<Vector4<double>>& Object::getVertices() const
            {
                return m_Vertices;
            }

            const std::vector<Vec3<double>>& Object::getNormals() const
            {
                return m_normals;
            }

            const std::vector<Index>& Object::getIndices() const
            {
                return m_Indices;
            }

            const Matrix4<double>& Object::getMatrix() const
            {
                return m_CacheModelMatrix;
            }

            void Object::scaleX(double amount)
            {
                m_ScaleVector[0] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::scaleY(double amount)
            {
                m_ScaleVector[1] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::scaleZ(double amount)
            {
                m_ScaleVector[2] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::scale(Vector4<double> amount)
            {
                m_ScaleVector = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::translateX(double amount)
            {
                m_TranslateVector[0] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::translateY(double amount)
            {
                m_TranslateVector[1] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::translateZ(double amount)
            {
                m_TranslateVector[2] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::translate(Vector4<double> amount)
            {
                m_TranslateVector = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::rotateX(double amount)
            {
                m_RotateVector[0] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::rotateY(double amount)
            {
                m_RotateVector[1] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::rotateZ(double amount)
            {
                m_RotateVector[2] = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            void Object::rotate(Vector4<double> amount)
            {
                m_RotateVector = amount;
                m_CacheModelMatrix = createModelMatrix(m_TranslateVector, m_RotateVector, m_ScaleVector);
            }

            Matrix4<double> Object::createModelMatrix(const Vector4<double>& translate, const Vector4<double>& rotate, const Vector4<double>& scale) const
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
        }
    }
}