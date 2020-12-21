#include "pch.h"
#include "SpecularMap.h"

namespace ModelViewer::Engine
{
    SpecularMap SpecularMap::fromTexture(const Texture& texture)
    {
        SpecularMap output = {};
        output.data.resize(texture.rawData.size() / texture.countChannels);
        output.width = texture.width;
        output.height = texture.height;

        for (std::size_t i = 0; i < output.data.size(); i++)
        {
            output.data[i] = static_cast<double>(texture.rawData[texture.countChannels * i]) / Color::MAX;
        }

        return output;
    }
}

