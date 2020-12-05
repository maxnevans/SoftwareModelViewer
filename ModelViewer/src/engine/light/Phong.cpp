#include "pch.h"
#include "Phong.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Light
        {
            Phong::Phong(AmbientLight ambientLight, DirectionalLight directionalLight)
                :
                m_ambient(ambientLight),
                m_directional(directionalLight)
            {
            }

            TriangleColorCalculator Phong::createTriangleLightCalculator(Primitives::FltTriangleRef triangleVertices,
                Primitives::FltTriangleRef triangleNormals) const
            {
                return [](const Vec4<double>& point, Color ownColor) -> Color
                {
                    return ownColor;
                };
            }
        }
    }
}