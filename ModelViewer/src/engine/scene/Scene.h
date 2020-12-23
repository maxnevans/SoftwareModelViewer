#pragma once
#include "Object.h"
#include "Camera.h"
#include "engine/Viewport.h"
#include "engine/Rasterizer.h"
#include "engine/light/Lambert.h"
#include "engine/DiffuseMap.h"
#include "engine/NormalMap.h"
#include "engine/SpecularMap.h"

namespace ModelViewer
{
    namespace Engine
    {
        namespace Scene
        {
            struct RenderResult
            {
                std::reference_wrapper<const std::vector<Vec4<double>>> ver;
                std::reference_wrapper<const std::vector<Vec4<double>>> verticesWorld;
                std::reference_wrapper<const std::vector<Vec3<double>>> uv;
                std::reference_wrapper<const std::vector<Index>> ind;
                const DiffuseMap& diffuseMap;
                const NormalMap& normalMap;
                const SpecularMap& specularMap;
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
                std::vector<Vector4<double>> m_verticesWorld;
                std::vector<Vec3<double>> m_textureVertices;
                std::vector<Index> m_indices;
                DiffuseMap m_diffuseMap;
                NormalMap m_normalMap;
                SpecularMap m_specularMap;
            };
        }
    }
}