#pragma once
#include "pch.h"
#include "engine/ObjectParser.h"
#include "math/Geometry.h"
#include "math/Vector.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Object.h"
#include "engine/scene/Camera.h"

namespace ModelViewer
{
    struct AdditionalDrawData
    {
        IntDimensions dimensions;
    };

    class ModelViewerApp
    {
    public:
        using OnLoadCallback = std::function<void(bool)>;

    public:
        ModelViewerApp();
        void start();
        bool stop(DWORD waitMilliseconds = INFINITE);
        bool didStop() const;
        void draw(Gdiplus::Bitmap& frame, const AdditionalDrawData& data);
        void rotateModelByX(double x);
        void rotateModelByY(double y);
        void rotateModelByZ(double z);
        void zoomIn(double zoom);
        void zoomOut(double zoom);
        void loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb = nullptr);

    private:
        Vector<double> m_RotateVector = {};
        double m_Zoom = 3.0;
        OnLoadCallback m_OnLoadCb = nullptr;
        std::shared_ptr<Engine::Scene::Object> m_Model = nullptr;
        std::shared_ptr<Engine::Scene::Scene> m_Scene = nullptr;
        std::shared_ptr<Engine::Viewport> m_Viewport = nullptr;
        std::shared_ptr<Engine::Scene::Camera> m_Camera = nullptr;
        std::shared_ptr<Engine::ParsedObject> m_ParsedObject = nullptr;
    };
}