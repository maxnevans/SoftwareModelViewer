#pragma once
#include "pch.h"
#include "ModelViewerApp.h"

namespace ModelViewer
{
    ModelViewerApp::ModelViewerApp()
    {
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

    void ModelViewerApp::draw(Gdiplus::Graphics& gfx, const AdditionalDrawData& data)
    {
        gfx.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(180, 180, 180)),
            Gdiplus::Rect(0, 0, data.dimensions.width, data.dimensions.height));
    }

    void ModelViewerApp::loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb)
    {
        if (cb)
            cb(true);
    }
}