#pragma once
#include "pch.h"
#include "Texture.h"
#include "Color.h"

namespace ModelViewer::Engine
{
    struct DiffuseMap
    {
        std::vector<Color> data;
        std::size_t width;
        std::size_t height;

        inline const Color& operator()(double u, double v) const
        {
            expect(u >= 0 && u <= 1);
            expect(v >= 0 && v <= 1);
            expect(width * height == data.size());
            const auto i = width * height - (static_cast<std::size_t>(v * height) * width + static_cast<std::size_t>(u * width));
            return data[i];
        }
        static DiffuseMap fromTexture(const Texture& texture);
    };
}