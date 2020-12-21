#pragma once
#include "pch.h"
#include "ModelViewerApp.h"
#include "engine/ObjectParser.h"
#include "engine/Viewport.h"
#include "engine/scene/Object.h"
#include "engine/scene/Camera.h"
#include "engine/scene/Scene.h"
#include "engine/Primitives.h"
#include "engine/ObjectParser.h"
#include "engine/TextureParser.h"
#include "engine/DiffuseMap.h"
#include "engine/NormalMap.h"
#include "engine/SpecularMap.h"

#define M_PI 3.14159265358979323846

namespace ModelViewer
{
    ModelViewerApp::ModelViewerApp(int width, int height)
        :
        m_RotateVector({0, 0, 0}),
        m_rasterizer(width, height),
        m_pool(std::thread::hardware_concurrency(), 0x8000)
    {
        m_Scene = std::make_shared<Engine::Scene::Scene>();

        m_Camera = std::make_shared<Engine::Scene::Camera>(Vector3<double>({ 0.0, 0.0, 3.0 }),
            Vector3<double>({ 0.0, 0.0, 0.0 }), Vector3<double>({ 0.0, 1.0, 0.0 }), M_PI / 2, static_cast<double>(width) / height, 0, 10.0);
        m_Scene->addCamera(m_Camera);

        m_Viewport = std::make_shared<Engine::Viewport>(0, 0, width, height);
    }

    void ModelViewerApp::start()
    {
    }

    bool ModelViewerApp::stop(DWORD waitMilliseconds)
    {
        return true;
    }

    bool ModelViewerApp::didStop() const
    {
        return true;
    }

    constexpr long long calcKey(int v1i, int v2i)
    {
        if (v1i > v2i)
            std::swap(v1i, v2i);

        return static_cast<long long>(v1i) << (sizeof(int) * 8) | v2i;
    }

    void ModelViewerApp::draw(Gdiplus::Graphics& gfx, const AdditionalDrawData& data)
    {
        auto&& [verRef, verticesWorldRef, normalsRef, uvsRef, indRef, colRef] = m_Scene->render(*m_Viewport);
        const auto& vertices = verRef.get();
        const auto& verticesWorld = verticesWorldRef.get();
        const auto& normals = normalsRef.get();
        const auto& uvs = uvsRef.get();
        const auto& indices = indRef.get();
        const auto& colors = colRef.get();

        const auto drawLine = [this, &data](std::reference_wrapper<const std::vector<Vector4<double>>> ver, std::size_t aInd, std::size_t bInd)
        {
            const auto& [screenWidth, screenHeight] = data.dimensions;
            std::optional clippedLine = Engine::Primitives::clipLine(0, 0, 
                screenWidth, screenHeight, std::pair{ ver.get()[aInd], ver.get()[bInd] });

            if (clippedLine)
            {
                const auto v1 = std::move((*clippedLine).first);
                const auto v2 = std::move((*clippedLine).second);

                m_rasterizer.drawLine(v1[X], v1[Y], v2[X], v2[Y], { 0xFF, 0xFF, 0xFF });
            }
        };

        const auto drawTriangle = [this, &data](std::reference_wrapper<const std::vector<Vec4<double>>> ver,
            std::reference_wrapper<const std::vector<Vec4<double>>> verticesWorld,
            std::reference_wrapper<const std::vector<Vec3<double>>> normals, 
            std::reference_wrapper<const std::vector<Vec3<double>>> uvs,
            std::reference_wrapper<const std::vector<Engine::Index>> indices, std::size_t indexSelector, 
            std::reference_wrapper<const Vector3<int>> cameraVector,
            Engine::Color color)
        {
            Engine::Index aInd = indices.get()[indexSelector];
            Engine::Index bInd = indices.get()[indexSelector + 1];
            Engine::Index cInd = indices.get()[indexSelector + 2];

            Engine::Primitives::FltTriangleRef triangle = {
                std::cref(ver.get()[aInd.vertex]),
                std::cref(ver.get()[bInd.vertex]),
                std::cref(ver.get()[cInd.vertex])
            };

            if (Engine::Primitives::isTriangleTowardsCamera(cameraVector.get(), triangle))
            {
                m_rasterizer.drawTriangle(ver.get()[aInd.vertex], ver.get()[aInd.vertex][Z], normals.get()[aInd.normal], verticesWorld.get()[aInd.vertex],
                    ver.get()[bInd.vertex], ver.get()[bInd.vertex][Z], normals.get()[bInd.normal], verticesWorld.get()[bInd.vertex],
                    ver.get()[cInd.vertex], ver.get()[cInd.vertex][Z], normals.get()[cInd.normal], verticesWorld.get()[cInd.vertex],
                    color);
            }
        };

        const auto cameraVector = static_cast<Vector3<int>>(m_Camera->getPosition() - m_Camera->getTarget());

        m_rasterizer.begin();

        for (size_t i = 0; i < indices.size(); i += 3)
        {
            std::size_t aInd = indices[i].vertex;
            std::size_t bInd = indices[i + 1].vertex;
            std::size_t cInd = indices[i + 2].vertex;

            if (vertices[aInd][Z] <= 0 || vertices[bInd][Z] <= 0 || vertices[cInd][Z] <= 0)
                continue;

            Engine::Color color = colors[0];

#if 0
            m_pool.enque(drawLine, std::cref(ver), aInd, bInd);
            m_pool.enque(drawLine, std::cref(ver), bInd, cInd);
            m_pool.enque(drawLine, std::cref(ver), cInd, aInd);
#elif 1
            //drawTriangle(verRef, verticesWorld, normalsRef, uvsRef, indRef, i, std::cref(cameraVector), color);
            m_pool.enque(drawTriangle, verRef, verticesWorld, normalsRef, uvsRef, indRef, i, std::cref(cameraVector), color);

#endif
        }

        m_pool.wait();

        m_rasterizer.end(gfx);
    }

    void ModelViewerApp::rotateModelByX(double x)
    {
        if (!m_Model)
            return;

        m_RotateVector[0] += x;
        m_Model->rotateX(m_RotateVector[0]);
    }

    void ModelViewerApp::rotateModelByY(double y)
    {
        if (!m_Model)
            return;

        m_RotateVector[1] += y;
        m_Model->rotateY(m_RotateVector[1]);
    }

    void ModelViewerApp::rotateModelByZ(double z)
    {
        if (!m_Model)
            return;

        m_RotateVector[2] += z;
        m_Model->rotateZ(m_RotateVector[2]);
    }

    void ModelViewerApp::zoomIn(double zoom)
    {
        m_Zoom -= zoom;
        //m_Camera->changePosition(Vector3<double>({ 0.0, 0.0, m_Zoom }));
        m_Model->scale(Vector4<double>({ m_Zoom, m_Zoom, m_Zoom }));
    }

    void ModelViewerApp::zoomOut(double zoom)
    {
        m_Zoom += zoom;
        //m_Camera->changePosition(Vector3<double>({ 0.0, 0.0, m_Zoom }));
        m_Model->scale(Vector4<double>({ m_Zoom, m_Zoom, m_Zoom }));
    }

    void ModelViewerApp::loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb)
    {
        Engine::ObjectParser parser(filename);

        auto object = parser.parse();
        m_Model = std::make_shared<Engine::Scene::Object>(std::move(object.vertices), std::move(object.normals),
            std::move(object.textureVertices), std::move(object.indices));
        m_Scene->addObject(m_Model);

        if (cb)
            cb(true);
    }

    void ModelViewerApp::loadDiffuseMapFromFile(const std::string& filename, OnLoadCallback cb)
    {
        Engine::TextureParser parser(filename);

        Engine::Texture diffuseMap = parser.parse();

        m_Model->setDiffuseMap(Engine::DiffuseMap::fromTexture(diffuseMap));

        if (cb)
            cb(true);
    }

    void ModelViewerApp::loadNormalMapFromFile(const std::string& filename, OnLoadCallback cb)
    {
        Engine::TextureParser parser(filename);

        Engine::Texture normalMap = parser.parse();

        m_Model->setNormalMap(Engine::NormalMap::fromTexture(normalMap));

        if (cb)
            cb(true);
    }

    void ModelViewerApp::loadSpecularMapFromFile(const std::string& filename, OnLoadCallback cb)
    {
        Engine::TextureParser parser(filename);

        Engine::Texture specularMap = parser.parse();

        m_Model->setSpecularMap(Engine::SpecularMap::fromTexture(specularMap));

        if (cb)
            cb(true);
    }

    void ModelViewerApp::setDimensions(int width, int height)
    {
        m_Camera->setAspectRatio(static_cast<double>(width) / height);
        m_Viewport->setDimensions(width, height);
    }

    void ModelViewerApp::modelBegin()
    {
    }

    void ModelViewerApp::modelEnd()
    {
    }
}