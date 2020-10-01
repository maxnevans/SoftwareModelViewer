#include "pch.h"
#include "Scene.h"
#include "Core.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
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
                m_indices.reserve(object->getIndices().size());
            }

            std::pair<std::reference_wrapper<const std::vector<Vector<int>>>, std::reference_wrapper<const std::vector<int>>> Scene::render(Viewport& vp)
            {
                expect(m_CurrentActiveCamera);

                m_vertices.clear();
                m_indices.clear();

                const auto vpv = vp.getMatrix() 
                    * m_CurrentActiveCamera->getProjectionMatrix() 
                    * m_CurrentActiveCamera->getViewMatrix();

                for (const auto& object : m_Objects)
                {
                    const auto& objVertices = object->getVertices();
                    const auto& objIndices = object->getIndices();

                    const auto vpvm = vpv * object->getMatrix();

                    for (auto vertex : objVertices)
                    {
                        auto transformedVertex = vpvm * vertex.rotateToColumn();
                        transformedVertex /= transformedVertex[3];
                        m_vertices.push_back(transformedVertex.staticCast<int>());
                    } 

                    m_indices.insert(m_indices.end(), objIndices.begin(), objIndices.end());
                }

                return {std::cref(m_vertices), std::cref(m_indices)};
            }
        }
    }
}
