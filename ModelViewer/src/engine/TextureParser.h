#pragma once
#include "pch.h"
#include "engine/Rasterizer.h"

namespace ModelViewer::Engine
{
    struct ParsedTexture
    {
        std::vector<ColorChannel> rawData;
        std::size_t width;
        std::size_t height;
    };

    class TextureParser
    {
    public:
        TextureParser(std::string filename);
        ParsedTexture parse();

    private:
        std::string m_filename;
    };
}

