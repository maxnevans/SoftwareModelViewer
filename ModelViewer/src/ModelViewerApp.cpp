#pragma once
#include "pch.h"
#include "ModelViewerApp.h"
#include "engine/Rasterizer.h"

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

    void ModelViewerApp::draw(Gdiplus::Bitmap& frame, const AdditionalDrawData& data)
    {
        Engine::Rasterizer gfx(frame);

        for (int i = 0; i < 100; i++)
            for (int j = 0; j < 100; j++)
                gfx.drawPixel(i, j, {0xAA, 0xAA, 0xAA});
    }

    void ModelViewerApp::loadMeshFromFile(const std::wstring& filename, OnLoadCallback cb)
    {
        if (cb)
            cb(true);
    }
}