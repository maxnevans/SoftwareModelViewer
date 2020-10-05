#pragma once
#include "pch.h"
#include "ModelViewerApp.h"
#include "engine/ObjectParser.h"
#include "engine/Viewport.h"
#include "engine/scene/Object.h"
#include "engine/scene/Camera.h"
#include "engine/scene/Scene.h"
#include "engine/Primitives.h"

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
            Vector3<double>({ 0.0, 0.0, 0.0 }), Vector3<double>({ 0.0, 1.0, 0.0 }), M_PI / 2, static_cast<double>(width) / height, 1.0, 10.0);
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
        auto&& [verRef, indRef] = m_Scene->render(*m_Viewport);
        const auto& ver = verRef.get();
        const auto& ind = indRef.get();

        auto drawLine = [this, &data](std::reference_wrapper<const std::vector<Vector4<int>>> ver, std::size_t aInd, std::size_t bInd)
        {
            const auto& [screenWidth, screenHeight] = data.dimensions;
            std::optional clipped = Engine::Primitives::clipLine(0, 0, screenWidth, screenHeight, std::pair{ ver.get()[aInd], ver.get()[bInd] });

            if (clipped)
            {
                constexpr int X = 0;
                constexpr int Y = 1;
                const auto v1 = std::move((*clipped).first);
                const auto v2 = std::move((*clipped).second);

                m_rasterizer.drawLine(v1[X], v1[Y], v2[X], v2[Y], { 0xFF, 0xFF, 0xFF });
            }
        };

        m_rasterizer.begin();

        for (size_t i = 0; i < ind.size(); i += 3)
        {
            expect(ind[i] != 0 && ind[i + 1] != 0 && ind[i + 2] != 0);

            size_t aInd = ind[i] < 0 ? ver.size() + ind[i] : ind[i] - 1;
            size_t bInd = ind[i + 1] < 0 ? ver.size() + ind[i + 1] : ind[i + 1] - 1;
            size_t cInd = ind[i + 2] < 0 ? ver.size() + ind[i + 2] : ind[i + 2] - 1;

            m_pool.enque(drawLine, std::cref(ver), aInd, bInd);
            m_pool.enque(drawLine, std::cref(ver), bInd, cInd);
            m_pool.enque(drawLine, std::cref(ver), cInd, aInd);
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
        m_Camera->changePosition(Vector3<double>({ 0.0, 0.0, m_Zoom }),
            Vector3<double>({ 0.0, 0.0, 0.0 }), Vector3<double>({ 0.0, 1.0, 0.0 }));
    }

    void ModelViewerApp::zoomOut(double zoom)
    {
        m_Zoom += zoom;
        m_Camera->changePosition(Vector3<double>({ 0.0, 0.0, m_Zoom }),
            Vector3<double>({ 0.0, 0.0, 0.0 }), Vector3<double>({ 0.0, 1.0, 0.0 }));
    }

    void ModelViewerApp::loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb)
    {
        Engine::ObjectParser parser(filename);

        auto object = parser.parse();
        m_Model = std::make_shared<Engine::Scene::Object>(std::move(object.vertices), std::move(object.indices));
        m_futures.resize(m_Model->getIndices().size());
        m_Scene->addObject(m_Model);

        if (cb)
            cb(true);
    }

    void ModelViewerApp::setDimensions(int width, int height)
    {
        m_Camera->setAspectRatio(static_cast<double>(width) / height);
        m_Viewport->setDimensions(width, height);
    }
}