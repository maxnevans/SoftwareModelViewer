#pragma once
#include "pch.h"
#include "engine/Color.h"
#include "engine/Texture.h"

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
        Texture parse();

    private:
        std::string m_filename;
    };
}

