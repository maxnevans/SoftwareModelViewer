#pragma once
#include "pch.h"
#include "engine/Texture.h"

namespace ModelViewer::Engine
{
    struct SpecularMap
    {
        std::vector<double> data;
        std::size_t width;
        std::size_t height;

        inline double operator()(double u, double v) const
        {
            expect(u >= 0 && u <= 1);
            expect(v >= 0 && v <= 1);
            expect(width * height == data.size());
            return data[static_cast<std::size_t>(v * height) * width + static_cast<std::size_t>(u * width)];
        }
        static SpecularMap fromTexture(const Texture& texture);
    };
}