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
            class Camera
            {
            public:
                Camera(Vector<double> position, Vector<double> target, Vector<double> upVector, double fov, double aspectRatio, double zNear, double zFar);
                const Matrix<double>& getViewMatrix() const;
                const Matrix<double>& getProjectionMatrix() const;
                void changePosition(Vector<double> position, Vector<double> target, Vector<double> upVector);

            private:
                Matrix<double> createViewMatrix(const Vector<double>& position, const Vector<double>& target, const Vector<double>& upVector) const;
                Matrix<double> createProjectionMatrixFromWidthAndHeight(double width, double height, double zNear, double zFar) const;
                Matrix<double> createProjectionMatrixFromFOVAndAspectRatio(double fov, double aspectRatio, double zNear, double zFar) const;

            private:
                Matrix<double> m_ViewMatrix;
                Matrix<double> m_ProjectionMatrix;
            };
        }
    }
}