#pragma once
#include "Object.h"
#include "Camera.h"
#include "engine/Viewport.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            class Scene
            {
            public:
                void addCamera(const std::shared_ptr<Camera>& camera);
                void addObject(const std::shared_ptr<Object>& object);
                std::pair<std::reference_wrapper<const std::vector<Vector4<int>>>, std::reference_wrapper<const std::vector<int>>> render(Viewport& vp);

            private:
                std::vector<std::shared_ptr<Camera>> m_Cameras;
                std::shared_ptr<Camera> m_CurrentActiveCamera;
                std::vector<std::shared_ptr<Object>> m_Objects;
                std::vector<Vector4<int>> m_vertices;
                std::vector<int> m_indices;
            };
        }
    }
}