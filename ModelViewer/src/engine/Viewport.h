#pragma once
#include "math/Matrix.h"

namespace ModelViewer
{
    namespace Engine
    {
        class Viewport
        {
        public:
            Viewport(int x, int y, int width, int height);
            const Matrix<double>& getMatrix() const;

        private:
            Matrix<double> createViewportMatrix(int x, int y, int width, int height) const;

        private:
            Matrix<double> m_ViewportMatrix;
        };
    }
}