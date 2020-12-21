#pragma once
#include "pch.h"
#include "Core.h"
#include "engine/Color.h"

namespace ModelViewer::Engine
{
    struct Texture
    {
        std::vector<ColorChannel> rawData;
        std::size_t width;
        std::size_t height;

        const std::size_t countChannels = 4;

        inline Color operator()(double u, double v) const
        {
            expect(u >= 0 && u <= 1);
            expect(v >= 0 && v <= 1);
            expect(width * height == rawData.size());

            const auto i = (static_cast<std::size_t>(v * height) * width + static_cast<std::size_t>(u * width)) * countChannels;
            return { rawData[i], rawData[i + 1], rawData[i + 2] };
        }
    };
}
