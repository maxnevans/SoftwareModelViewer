#pragma once
#include "Object.h"
#include "Camera.h"
#include "engine/Viewport.h"
#include "engine/Rasterizer.h"
#include "engine/light/Lambert.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            struct RenderResult
            {
                std::reference_wrapper<const std::vector<Vec4<double>>> ver;
                std::reference_wrapper<const std::vector<Vec3<double>>> normals;
                std::reference_wrapper<const std::vector<Vec3<double>>> uv;
                std::reference_wrapper<const std::vector<Index>> ind;
                std::reference_wrapper<const std::vector<Engine::Color>> col;
            };

            class Scene
            {
            public:
                Scene();
                void addCamera(const std::shared_ptr<Camera>& camera);
                void addObject(const std::shared_ptr<Object>& object);
                RenderResult render(Viewport& vp);

            private:
                std::vector<std::shared_ptr<Camera>> m_Cameras;
                std::shared_ptr<Camera> m_CurrentActiveCamera;
                std::vector<std::shared_ptr<Object>> m_Objects;
                std::vector<Vector4<double>> m_vertices;
                std::vector<Vec3<double>> m_textureVertices;
                std::vector<Vec3<double>> m_normals;
                std::vector<Index> m_indices;
                std::vector<Engine::Color> m_colors;
                Light::Lambert m_light;
            };
        }
    }
}