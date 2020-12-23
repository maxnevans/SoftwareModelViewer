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
                const Vec3<double>& view, Color objectBaseColor, double ks) const
            {
                const Vec3<double> lightDirection = m_directional.direction.normalize();

                // TODO: remove hardcode viewPosition
                const Vec3<double> viewPosition = { { 0.0, 0.0, -3.0 } };
                const Vec3<double> viewDirection = viewPosition - worldVertex;

                const double cosTheta = (std::max)(0.0, normal.cos(lightDirection));

                constexpr double ka = 0.5;
                constexpr double kd = 1;
                constexpr double sh = 1;
                
                Vec3<double> ambient = static_cast<Vec3<double>>(m_ambient.color) * ka;
                Vec3<double> diffuse = Vec3<double>{};
                Vec3<double> specular = Vec3<double>{};

                if (cosTheta > 0)
                {
                    Vec3<double> reflectionDirection = (lightDirection - normal * (lightDirection.dotProduct(normal)) * 2).normalize();

                    diffuse = static_cast<Vec3<double>>(m_directional.color) * cosTheta * kd;

                    specular = static_cast<Vec3<double>>(m_directional.color) 
                        * std::pow((std::max)(0.0, reflectionDirection.dotProduct(viewDirection)), sh) * ks;
                }

                Vec3<double> fragmentColor = ((ambient + diffuse + specular) / Color::MAX).componentwiseMultiplication(
                    static_cast<Vec3<double>>(objectBaseColor));
                /*Vec3<double> fragmentColor = ((specular) / Color::MAX).componentwiseMultiplication(
                    static_cast<Vec3<double>>(objectBaseColor));*/

                return { 
                    Color::boundColorChannel(fragmentColor[0]), 
                    Color::boundColorChannel(fragmentColor[1]), 
                    Color::boundColorChannel(fragmentColor[2]) 
                };
                //return objectBaseColor;
            }
        }
    }
}