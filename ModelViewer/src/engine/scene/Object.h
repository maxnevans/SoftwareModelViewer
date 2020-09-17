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
                Object(std::vector<Vector<double>> vertices, std::vector<int> indices);
                const std::vector<Vector<double>>& getVertices() const;
                const std::vector<int>& getIndices() const;
                const Matrix<double>& getMatrix() const;
                void scaleX(double amount);
                void scaleY(double amount);
                void scaleZ(double amount);
                void scale(Vector<double> amount);
                void translateX(double amount);
                void translateY(double amount);
                void translateZ(double amount);
                void translate(Vector<double> amount);
                void rotateX(double amount);
                void rotateY(double amount);
                void rotateZ(double amount);
                void rotate(Vector<double> amount);

            private:
                Matrix<double> createModelMatrix(const Vector<double>& translate, const Vector<double>& rotate, const Vector<double>& scale) const;

            private:
                std::vector<Vector<double>> m_Vertices;
                std::vector<int> m_Indices;
                Vector<double> m_TranslateVector;
                Vector<double> m_RotateVector;
                Vector<double> m_ScaleVector;
                Matrix<double> m_CacheModelMatrix;
            };
        }
    }
}