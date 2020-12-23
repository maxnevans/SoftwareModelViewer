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
                m_indices.reserve(object->getIndices().size());

                /*m_diffuseMap = {};
                m_diffuseMap.data.reserve(object->getDiffuseMap().data.size());
                m_diffuseMap.width = object->getDiffuseMap().width;
                m_diffuseMap.height = object->getDiffuseMap().height;*/
                m_diffuseMap = object->getDiffuseMap();

                m_normalMap = {};
                m_normalMap.data.reserve(object->getNormalMap().data.size());
                m_normalMap.data.resize(object->getNormalMap().data.size());
                m_normalMap.width = object->getNormalMap().width;
                m_normalMap.height = object->getNormalMap().height;

                /*m_specularMap = {};
                m_specularMap.data.reserve(object->getSpecularMap().data.size());
                m_specularMap.width = object->getSpecularMap().width;
                m_specularMap.height = object->getSpecularMap().height;*/
                m_specularMap = object->getSpecularMap();
            }

            RenderResult Scene::render(Viewport& vp)
            {
                expect(m_CurrentActiveCamera);

                /*m_vertices.clear();
                m_verticesWorld.clear();
                m_indices.clear();*/

                const auto vpv = vp.getMatrix() 
                    * m_CurrentActiveCamera->getProjectionMatrix() 
                    * m_CurrentActiveCamera->getViewMatrix();
                const auto& v = m_CurrentActiveCamera->getViewMatrix();

                for (const auto& object : m_Objects)
                {
                    const auto& objVertices = object->getVertices();
                    const auto& objTextureVertices = object->getTextureVertices();
                    const auto& objIndices = object->getIndices();
                    const auto& objNormalMap = object->getNormalMap();

                    m_vertices.resize(objVertices.size());
                    m_verticesWorld.resize(objVertices.size());
                    m_textureVertices.resize(objTextureVertices.size());
                    m_indices.resize(objIndices.size());

                    const auto& m = object->getMatrix();
                    const Mat3<double> mNormal = static_cast<Mat3<double>>(object->getNormalMatrix());

                    // Copy vertices
                    for (std::size_t i = 0; i < objVertices.size(); i++)
                        m_vertices[i] = objVertices[i];

                    // Copy normals
                    for (std::size_t i = 0; i < objNormalMap.data.size(); i++)
                        m_normalMap.data[i] = objNormalMap.data[i];

                    // Copy indices
                    for (std::size_t i = 0; i < objIndices.size(); i++)
                        m_indices[i] = objIndices[i];

                    // Copy texture vertices (UVs)
                    for (std::size_t i = 0; i < objTextureVertices.size(); i++)
                        m_textureVertices[i] = objTextureVertices[i];

                    // Model matrix applying
                    for (std::size_t i = 0; i < m_vertices.size(); i++)
                    {
                        m_vertices[i] = m * m_vertices[i];
                        m_verticesWorld[i] = m_vertices[i];
                    }

                    // Model matrix applying for normals
                    for (std::size_t i = 0; i < m_normalMap.data.size(); i++)
                        m_normalMap.data[i] = mNormal * m_normalMap.data[i];

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
                    std::cref(m_textureVertices),
                    std::cref(m_indices),
                    m_diffuseMap,
                    m_normalMap,
                    m_specularMap
                };
            }
        }
    }
}
