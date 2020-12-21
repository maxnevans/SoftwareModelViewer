#pragma once
#include "engine/Color.h"
#include "math/Vector.h"
#include "engine/Primitives.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Light
        {
            struct AmbientLight
            {
                Color color;
                double intensity;
            };

            struct DirectionalLight
            {
                Vector3<double> direction;
                Color color;
                double intensity;
            };

            using TriangleColorCalculator = std::function<Color(const Vec4<double>&, Color)>;

            class Phong
            {
            public:
                Phong(AmbientLight ambientLight, DirectionalLight directionalLight);
                Color operator()(const Vec3<double>& normal, const Vec3<double>& worldVertex, 
                    const Vec3<double>& view, Color objectBaseColor, double ks = 1.0) const;

            private:
                AmbientLight m_ambient;
                DirectionalLight m_directional;
            };
        }
    }
}
