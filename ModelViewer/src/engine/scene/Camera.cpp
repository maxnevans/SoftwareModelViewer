#include "pch.h"
#include "Camera.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            Camera::Camera(Vector<double> position, Vector<double> target, Vector<double> upVector, double fov, double aspectRatio, double zNear, double zFar)
                :
                m_ViewMatrix(createViewMatrix(position, target, upVector)),
                m_ProjectionMatrix(createProjectionMatrixFromFOVAndAspectRatio(fov, aspectRatio, zNear, zFar)),
                m_position(std::move(position)),
                m_target(std::move(target)),
                m_upVector(std::move(upVector)),
                m_fov(fov),
                m_aspectRatio(aspectRatio),
                m_zNear(zNear),
                m_zFar(zFar)
            {
            }

            const Matrix<double>& Camera::getViewMatrix()
            {
                if (m_shouldUpdateViewMatrix)
                {
                    m_shouldUpdateViewMatrix = false;
                    m_ViewMatrix = createViewMatrix(m_position, m_target, m_upVector);
                }

                return m_ViewMatrix;
            }

            const Matrix<double>& Camera::getProjectionMatrix()
            {
                if (m_shouldUpdateProjectionMatrix)
                {
                    m_shouldUpdateProjectionMatrix = false;
                    m_ProjectionMatrix = createProjectionMatrixFromFOVAndAspectRatio(m_fov, m_aspectRatio, m_zNear, m_zFar);
                }

                return m_ProjectionMatrix;
            }

            void Camera::changePosition(Vector<double> position, Vector<double> target, Vector<double> upVector)
            {
                m_ViewMatrix = createViewMatrix(position, target, upVector);
            }

            void Camera::setAspectRatio(double ratio)
            {
                m_shouldUpdateProjectionMatrix = true;
                m_aspectRatio = ratio;
            }

            Matrix<double> Camera::createViewMatrix(const Vector<double>& position, const Vector<double>& target, const Vector<double>& upVector) const
            {
                auto zAxis = (position - target).normalize();
                auto xAxis = upVector.crossProduct(zAxis).normalize();
                auto yAxis = upVector.normalize();
                return Matrix<double>({
                    xAxis[0],   xAxis[1],   xAxis[2],   -(xAxis.dotProduct(position)),
                    yAxis[0],   yAxis[1],   yAxis[2],   -(yAxis.dotProduct(position)),
                    zAxis[0],   zAxis[1],   zAxis[2],   -(zAxis.dotProduct(position)),
                    0,          0,          0,          1
                }, 4, 4);
            }

            Matrix<double> Camera::createProjectionMatrixFromWidthAndHeight(double width, double height, double zNear, double zFar) const
            {
                return Matrix<double>({
                    2.0 * zNear / width,    0,                          0,                                  0,
                    0,                      2.0 * zNear / height,       0,                                  0,
                    0,                      0,                          zFar / (zNear - zFar),              zNear * zFar / (zNear - zFar),
                    0,                      0,                          -1,                                 0
                }, 4, 4);
            }
            Matrix<double> Camera::createProjectionMatrixFromFOVAndAspectRatio(double fov, double aspectRatio, double zNear, double zFar) const
            {
                double tan = std::tan(fov / 2);
                double zRange = zNear - zFar;

                return Matrix<double>({
                    1 / (aspectRatio * tan),        0,              0,                  0,
                    0,                              1 / tan,        0,                  0,
                    0,                              0,              zFar / zRange,      zNear * zFar / zRange,
                    0,                              0,             -1,                  0
                }, 4, 4);
            }
        }
    }
}