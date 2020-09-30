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
            const Matrix<double>& getMatrix();
            void setDimensions(int width, int height);
            void setAnchorPoint(int x, int y);

        private:
            Matrix<double> createViewportMatrix(int x, int y, int width, int height) const;

        private:
            Matrix<double> m_ViewportMatrix;
            bool m_shouldUpdateCache = false;
            int m_x;
            int m_y;
            int m_width;
            int m_height;
        };
    }
}