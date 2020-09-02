#pragma once
#include "pch.h"
#include "Geometry.h"

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
        void draw(Gdiplus::Graphics& gfx, const AdditionalDrawData& data);
        void loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb = nullptr);

    private:
        OnLoadCallback m_OnLoadCb = nullptr;
    };
}