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

            Color Phong::operator()(const Vec3<double>& normal, const Vec3<double>& worldVertex, 
                const Vec3<double>& view, Color objectBaseColor) const
            {
                const Vec3<double> lightDirection = m_directional.direction.normalize();

                // TODO: remove hardcode viewPosition
                const Vec3<double> viewPosition = { { 0.0, 0.0, -3.0 } };
                const Vec3<double> viewDirection = viewPosition - worldVertex;

                const double cosTheta = (std::max)(0.0, normal.cos(lightDirection));

                constexpr double ke = 0.1;
                constexpr double ka = 0.2;
                constexpr double kd = 0.3;
                constexpr double ks = 1;
                constexpr double sh = 4;
                
                Vec3<double> emissive = static_cast<Vec3<double>>(objectBaseColor) * ke;
                Vec3<double> ambient = static_cast<Vec3<double>>(m_ambient.color) * ka;
                Vec3<double> diffuse = Vec3<double>{};
                Vec3<double> specular = Vec3<double>{};

                if (cosTheta > 0)
                {
                    Vec3<double> reflectionDirection = lightDirection - normal * (lightDirection.dotProduct(normal)) * 2;

                    diffuse = static_cast<Vec3<double>>(m_directional.color) * cosTheta * kd;

                    specular = static_cast<Vec3<double>>(m_directional.color) 
                        * std::pow((std::max)(0.0, reflectionDirection.cos(viewDirection)), sh) * ks;
                }

                Vec3<double> total = emissive + ambient + diffuse + specular;

                return { Color::boundColorChannel(total[0]), Color::boundColorChannel(total[1]), Color::boundColorChannel(total[2]) };
            }
        }
    }
}