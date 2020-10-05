#include "pch.h"
#include "Viewport.h"

namespace ModelViewer
{
    namespace Engine
    {
        Viewport::Viewport(int x, int y, int width, int height)
            :
            m_ViewportMatrix(createViewportMatrix(x, y, width, height)),
            m_x(x),
            m_y(y),
            m_width(width),
            m_height(height)
        {
        }

        const Matrix4<double>& Viewport::getMatrix()
        {
            if (m_shouldUpdateCache)
            {
                m_ViewportMatrix = createViewportMatrix(m_x, m_y, m_width, m_height);
                m_shouldUpdateCache = false;
            }

            return m_ViewportMatrix;
        }

        Matrix4<double> Viewport::createViewportMatrix(int x, int y, int width, int height) const
        {
            double halfWidth = width / 2.0;
            double halfHeight = height / 2.0;

            return Matrix4<double>({
                halfWidth,      0,              0,      x + halfWidth,
                0,              -halfHeight,    0,      y + halfHeight,
                0,              0,              1,      0,
                0,              0,              0,      1
            });
        }

        void Viewport::setDimensions(int width, int height)
        {
            m_shouldUpdateCache = true;
            m_width = width;
            m_height = height;
        }

        void Viewport::setAnchorPoint(int x, int y)
        {
            m_shouldUpdateCache = true;
            m_x = x;
            m_y = y;
        }
    }
}