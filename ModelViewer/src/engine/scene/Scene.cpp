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
            }

            std::pair<std::vector<Vector<int>>, std::vector<int>> Scene::render(Viewport& vp)
            {
                expect(m_CurrentActiveCamera);

                std::vector<Vector<int>> vertices;
                std::vector<int> indices;

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

                        vertices.push_back(r5.staticCast<int>());
                    } 

                    indices.insert(indices.end(), objIndices.begin(), objIndices.end());
                }

                return {vertices, indices};
            }
        }
    }
}
