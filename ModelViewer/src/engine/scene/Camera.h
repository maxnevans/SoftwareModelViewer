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
                Camera(Vector3<double> position, Vector3<double> target, Vector3<double> upVector, double fov, double aspectRatio, double zNear, double zFar);
                const Matrix4<double>& getViewMatrix();
                const Matrix4<double>& getProjectionMatrix();
                void changePosition(Vector3<double> position, Vector3<double> target, Vector3<double> upVector);
                void setAspectRatio(double ratio);

            private:
                Matrix4<double> createViewMatrix(const Vector3<double>& position, const Vector3<double>& target, const Vector3<double>& upVector) const;
                Matrix4<double> createProjectionMatrixFromWidthAndHeight(double width, double height, double zNear, double zFar) const;
                Matrix4<double> createProjectionMatrixFromFOVAndAspectRatio(double fov, double aspectRatio, double zNear, double zFar) const;

            private:
                Matrix4<double> m_ViewMatrix;
                Matrix4<double> m_ProjectionMatrix;
                Vector3<double> m_position;
                Vector3<double> m_target;
                Vector3<double> m_upVector;
                bool m_shouldUpdateViewMatrix = false;
                bool m_shouldUpdateProjectionMatrix = false;
                double m_fov;
                double m_aspectRatio;
                double m_zNear;
                double m_zFar;
            };
        }
    }
}