#include "pch.h"
#include "NormalMap.h"

namespace ModelViewer::Engine
{
    NormalMap NormalMap::fromTexture(const Texture& texture)
    {
        NormalMap output = {};
        output.data.resize(texture.rawData.size() / texture.countChannels);
        output.width = texture.width;
        output.height = texture.height;

        for (std::size_t i = 0; i < output.data.size(); i++)
        {
            output.data[i] =  Vec3<double>({ 
                colorChannelToNormalized(texture.rawData[texture.countChannels * i]),
                colorChannelToNormalized(texture.rawData[texture.countChannels * i + 1]), 
                colorChannelToNormalized(texture.rawData[texture.countChannels * i + 2]) 
            });
        }

        return output;
    }
    double NormalMap::colorChannelToNormalized(const ColorChannel channel)
    {
        return static_cast<double>(channel) / Color::MAX * 2 - 1;
    }
}

