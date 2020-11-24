#include "pch.h"
#include "Lambert.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Light
        {
            Lambert::Lambert(Vector3<double> position, Color color, double intensity)
                :
                m_position(position),
                m_color(color),
                m_intensity(intensity)
            {
            }

            Color Lambert::operator()(Color emissiveColor, Primitives::FltTriangle triangle) const
            {
                const auto polygonNormal = Primitives::calcTriangleNormal(triangle);

                const auto redReflectionFactor = static_cast<double>(emissiveColor.r) / Color::MAX;
                const auto greenReflectionFactor = static_cast<double>(emissiveColor.g) / Color::MAX;
                const auto blueReflectionFactor = static_cast<double>(emissiveColor.b) / Color::MAX;

                const auto cosAngle = polygonNormal.cos(m_position);
                const auto normalAngleFactor = cosAngle > 0 ? cosAngle : 0;

                Color color{};

                for (int i = 0; i < 3; i++)
                {
                    const auto distanceSquared = (triangle[i].get() - m_position).lengthSquared();
                    const auto distanceIntensityFactor = 1 / (1 + distanceSquared);

                    const auto generalFactor = distanceIntensityFactor * m_intensity * normalAngleFactor;

                    color.r += static_cast<unsigned char>(m_color.r * redReflectionFactor * generalFactor / 3.0);
                    color.g += static_cast<unsigned char>(m_color.g * redReflectionFactor * generalFactor / 3.0);
                    color.b += static_cast<unsigned char>(m_color.b * redReflectionFactor * generalFactor / 3.0);
                }

                return color;
            }
        }
    }
}