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

            unsigned char boundColorChannel(double newColorChannel)
            {
                return static_cast<unsigned char>((std::max)((std::min)(newColorChannel, static_cast<double>(Color::MAX)), 
                    static_cast<double>(Color::MIN)));
            }

            Color Lambert::operator()(Color emissiveColor, Primitives::FltTriangleRef triangle) const
            {
                const auto polygonNormal = Primitives::calcTriangleNormal(triangle);

                const auto redReflectionFactor = static_cast<double>(emissiveColor.r) / Color::MAX;
                const auto greenReflectionFactor = static_cast<double>(emissiveColor.g) / Color::MAX;
                const auto blueReflectionFactor = static_cast<double>(emissiveColor.b) / Color::MAX;

                const auto cosAngle = polygonNormal.cos(m_position);
                const auto normalAngleFactor = cosAngle > 0 ? cosAngle : 0;

                double color[3]{};

                for (int i = 0; i < 3; i++)
                {
                    auto diffVec = triangle[i].get() - m_position;
                    diffVec[3] = 0;
                    const auto distanceSquared = diffVec.lengthSquared();
                    const auto distanceIntensityFactor = 1 / (1 + distanceSquared);

                    const auto generalFactor = distanceIntensityFactor * m_intensity * normalAngleFactor;

                    color[0] += m_color.r * redReflectionFactor * generalFactor;
                    color[1] += m_color.g * greenReflectionFactor * generalFactor;
                    color[2] += m_color.b * blueReflectionFactor * generalFactor;
                }

                for (int i = 0; i < 3; i++)
                    color[i] /= 3;

                return {
                    boundColorChannel(color[0]),
                    boundColorChannel(color[1]),
                    boundColorChannel(color[2]),
                };
            }
        }
    }
}