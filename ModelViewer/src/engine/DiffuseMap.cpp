#include "pch.h"
#include "DiffuseMap.h"
#include "Core.h"

namespace ModelViewer::Engine
{
    DiffuseMap DiffuseMap::fromTexture(const Texture& texture)
    {
        DiffuseMap output = {};
        output.data.resize(texture.rawData.size() / texture.countChannels);
        output.width = texture.width;
        output.height = texture.height;

        for (std::size_t i = 0; i < output.data.size(); i++)
            output.data[i] = Color{ texture.rawData[texture.countChannels * i], 
                texture.rawData[texture.countChannels * i + 1], texture.rawData[texture.countChannels * i + 2] };

        return output;
    }
}

