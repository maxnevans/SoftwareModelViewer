#pragma once
#include "pch.h"
#include "ModelViewerApp.h"
#include "engine/ObjectParser.h"
#include "engine/Rasterizer.h"
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
        m_RotateVector({0, 0, 0})
    {
        /*std::vector<double> verticesRaw = {
            -0.5,   -0.5,  -0.5,   1,
             0.5,   -0.5,  -0.5,   1,
             0.5,    0.5,  -0.5,   1,
            -0.5,    0.5,  -0.5,   1,
            -0.5,   -0.5,   0.5,   1,
             0.5,   -0.5,   0.5,   1,
             0.5,    0.5,   0.5,   1,
            -0.5,    0.5,   0.5,   1
        };*/

        //std::vector<int> indices = {
            //1, 2, 2, 3, 3, 4, 4, 1, 1, 5, 2, 6, 5, 6, 5, 8, 8, 7, 6, 7, 5, 6
        //    0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7
        //};

        //std::vector<Vector<double>> vertices;
        //for (size_t i = 0; i < verticesRaw.size() / 4; i++)
        //    vertices.emplace_back(std::valarray<double>{ verticesRaw[4 * i], verticesRaw[4 * i + 1], verticesRaw[4 * i + 2], verticesRaw[4 * i + 3]});

        //m_Model = std::make_shared<Engine::Scene::Object>(vertices, indices);
        //m_Model->translate(Vector<double>({0, 0, 0}));
        //m_Model->scale(Vector<double>({2, 2, 2}));

        m_Scene = std::make_shared<Engine::Scene::Scene>();

        m_Camera = std::make_shared<Engine::Scene::Camera>(Vector<double>({ 0.0, 0.0, 3.0 }),
            Vector<double>({ 0.0, 0.0, 0.0 }), Vector<double>({ 0.0, 1.0, 0.0 }), M_PI / 2, static_cast<double>(width) / height, 1.0, 10.0);
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
        return static_cast<long long>(v1i) << sizeof(int) | v2i;
    }

    void ModelViewerApp::draw(Gdiplus::Bitmap& frame, const AdditionalDrawData& data)
    {
        Engine::Rasterizer gfx(frame);

        auto [ver, ind] = m_Scene->render(*m_Viewport);
        /*std::vector<std::future<void>> futures;
        futures.reserve(ind.size() / 3);*/

        const auto screenWidth = gfx.getWidth();
        const auto screenHeight = gfx.getHeight();

        auto drawLine = [&gfx, &screenWidth, &screenHeight](std::reference_wrapper<std::vector<Vector<int>>> ver, std::size_t aInd, std::size_t bInd)
        {
            std::optional clipped = Engine::Primitives::clipLine(0, 0, screenWidth, screenHeight, std::pair{ ver.get()[aInd], ver.get()[bInd] });

            if (clipped)
                gfx.drawLine((*clipped).first[0], (*clipped).first[1], (*clipped).second[0], (*clipped).second[1], { 0xFF, 0xFF, 0xFF });
        };

        for (size_t i = 0; i < ind.size(); i += 3)
        {
            expect(ind[i] != 0 && ind[i + 1] != 0 && ind[i + 2] != 0);

            size_t aInd = ind[i] < 0 ? ver.size() + ind[i] : ind[i] - 1;
            size_t bInd = ind[i + 1] < 0 ? ver.size() + ind[i + 1] : ind[i + 1] - 1;
            size_t cInd = ind[i + 2] < 0 ? ver.size() + ind[i + 2] : ind[i + 2] - 1;

            drawLine(ver, aInd, bInd);
            drawLine(ver, bInd, cInd);
            drawLine(ver, cInd, aInd);

            /*futures.push_back(std::async(std::launch::async, drawLine, std::ref(ver), aInd, bInd));
            futures.push_back(std::async(std::launch::async, drawLine, std::ref(ver), bInd, cInd));
            futures.push_back(std::async(std::launch::async, drawLine, std::ref(ver), cInd, aInd));*/
        }
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
        m_Camera->changePosition(Vector<double>({ 0.0, 0.0, m_Zoom }),
            Vector<double>({ 0.0, 0.0, 0.0 }), Vector<double>({ 0.0, 1.0, 0.0 }));
    }

    void ModelViewerApp::zoomOut(double zoom)
    {
        m_Zoom += zoom;
        m_Camera->changePosition(Vector<double>({ 0.0, 0.0, m_Zoom }),
            Vector<double>({ 0.0, 0.0, 0.0 }), Vector<double>({ 0.0, 1.0, 0.0 }));
    }

    void ModelViewerApp::loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb)
    {
        Engine::ObjectParser parser(filename);

        auto object = parser.parse();
        m_Model = std::make_shared<Engine::Scene::Object>(std::move(object.vertices), std::move(object.indices));
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