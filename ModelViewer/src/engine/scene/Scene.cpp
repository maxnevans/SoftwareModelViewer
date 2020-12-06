#include "pch.h"
#include "Scene.h"
#include "Core.h"
#include "engine/Primitives.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            Scene::Scene()
                :
                m_light(Vector3<double>({ 0, 5.0, 5.0 }), { 150, 20, 20 }, 50)
            {
            }

            void Scene::addCamera(const std::shared_ptr<Camera>& camera)
            {
                expect(camera);

                m_Cameras.push_back(camera);
                if (m_CurrentActiveCamera == nullptr)
                    m_CurrentActiveCamera = m_Cameras.back();
            }

            void Scene::addObject(const std::shared_ptr<Object>& object)
            {
                expect(object);

                m_Objects.push_back(object);

                m_vertices.reserve(object->getVertices().size());
                m_verticesWorld.reserve(object->getVertices().size());
                m_normals.reserve(object->getNormals().size());
                m_indices.reserve(object->getIndices().size());
                m_colors.reserve(object->getIndices().size());
            }

            RenderResult Scene::render(Viewport& vp)
            {
                expect(m_CurrentActiveCamera);

                m_vertices.clear();
                m_verticesWorld.clear();
                m_normals.clear();
                m_indices.clear();
                m_colors.clear();

                const auto vpv = vp.getMatrix() 
                    * m_CurrentActiveCamera->getProjectionMatrix() 
                    * m_CurrentActiveCamera->getViewMatrix();
                const auto& v = m_CurrentActiveCamera->getViewMatrix();

                for (const auto& object : m_Objects)
                {
                    const auto& objVertices = object->getVertices();
                    const auto& objTextureVertices = object->getTextureVertices();
                    const auto& objNormals = object->getNormals();
                    const auto& objIndices = object->getIndices();
                    const auto& objColors = object->getColors();

                    m_vertices.resize(objVertices.size());
                    m_verticesWorld.resize(objVertices.size());
                    m_normals.resize(objNormals.size());
                    m_textureVertices.resize(objTextureVertices.size());
                    m_indices.resize(objIndices.size());
                    m_colors.resize(objIndices.size());

                    const auto& m = object->getMatrix();
                    const Mat3<double> mNormal = static_cast<Mat3<double>>(object->getNormalMatrix());

                    // Copy vertices
                    for (std::size_t i = 0; i < objVertices.size(); i++)
                        m_vertices[i] = objVertices[i];

                    // Copy normals
                    for (std::size_t i = 0; i < objNormals.size(); i++)
                        m_normals[i] = objNormals[i];

                    // Copy colors
                    for (std::size_t i = 0; i < objColors.size(); i++)
                        m_colors[i] = objColors[i];

                    // Copy indices
                    for (std::size_t i = 0; i < objIndices.size(); i++)
                        m_indices[i] = objIndices[i];

                    // Model matrix applying
                    for (std::size_t i = 0; i < m_vertices.size(); i++)
                    {
                        m_vertices[i] = m * m_vertices[i];
                        m_verticesWorld[i] = m_vertices[i];
                    }

                    // Model matrix applying for normals
                    for (std::size_t i = 0; i < m_normals.size(); i++)
                        m_normals[i] = mNormal * m_normals[i];

                    // View Projective Viewport matrix applying and dividing by W
                    for (std::size_t i = 0; i < m_vertices.size(); i++)
                    {
                        m_vertices[i] = vpv * m_vertices[i];
                        m_vertices[i][X] /= m_vertices[i][W];
                        m_vertices[i][Y] /= m_vertices[i][W];
                    }
                }

                return {
                    std::cref(m_vertices), 
                    std::cref(m_verticesWorld), 
                    std::cref(m_normals),
                    std::cref(m_textureVertices),
                    std::cref(m_indices),
                    std::cref(m_colors),
                };
            }
        }
    }
}
