#pragma once
#include "pch.h"
#include "math/Vector.h"
#include "math/Matrix.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            class Object
            {
            public:
                Object(std::vector<Vector4<double>> vertices, std::vector<int> indices);
                const std::vector<Vector4<double>>& getVertices() const;
                const std::vector<int>& getIndices() const;
                const Matrix4<double>& getMatrix() const;
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

            private:
                Matrix4<double> createModelMatrix(const Vector4<double>& translate, const Vector4<double>& rotate, const Vector4<double>& scale) const;

            private:
                std::vector<Vector4<double>> m_Vertices;
                std::vector<int> m_Indices;
                Vector4<double> m_TranslateVector;
                Vector4<double> m_RotateVector;
                Vector4<double> m_ScaleVector;
                Matrix4<double> m_CacheModelMatrix;
            };
        }
    }
}