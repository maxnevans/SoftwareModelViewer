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

                for (const auto& object : m_Objects)
                {
                    //auto mvpv = vpv * object->getMatrix();
                    const auto& objVertices = object->getVertices();
                    const auto& objIndices = object->getIndices();

                    for (auto vertex : objVertices)
                    {
                        auto r1 = object->getMatrix() * vertex.rotateToColumn();
                        auto r2 = m_CurrentActiveCamera->getViewMatrix() * r1;
                        auto r3 = m_CurrentActiveCamera->getProjectionMatrix() * r2;
                        auto r4 = r3 / r3[3]; // Divide by w coordinate
                        auto r5 = vp.getMatrix() * r4;

                        m_vertices.push_back(r5.staticCast<int>());
                    } 

                    m_indices.insert(m_indices.end(), objIndices.begin(), objIndices.end());
                }

                return {std::cref(m_vertices), std::cref(m_indices)};
            }
        }
    }
}
