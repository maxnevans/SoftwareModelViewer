#include "pch.h"
#include "Viewport.h"

namespace ModelViewer
{
    namespace Engine
    {
        Viewport::Viewport(int x, int y, int width, int height)
            :
            m_ViewportMatrix(createViewportMatrix(x, y, width, height))
        {
        }

        const Matrix<double>& Viewport::getMatrix() const
        {
            return m_ViewportMatrix;
        }

        Matrix<double> Viewport::createViewportMatrix(int x, int y, int width, int height) const
        {
            double halfWidth = width / 2.0;
            double halfHeight = height / 2.0;

            return Matrix<double>({
                halfWidth,      0,              0,      x + halfWidth,
                0,              -halfHeight,    0,      y + halfHeight,
                0,              0,              1,      0,
                0,              0,              0,      1
            }, 4, 4);
        }
    }
}