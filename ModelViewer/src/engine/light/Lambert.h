#pragma once
#include "engine/Rasterizer.h"
#include "math/Vector.h"
#include "engine/Primitives.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Light
        {
            class Lambert
            {
            public:
                Lambert(Vector3<double> position, Color color, double intensity);
                Color operator()(Color emissiveColor, Primitives::FltTriangleRef triangle) const;

            private:
                Color m_color;
                Vector3<double> m_position;
                double m_intensity;
            };
        }
    }
}