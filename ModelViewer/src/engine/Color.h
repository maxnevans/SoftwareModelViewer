#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer::Engine
{
    using ColorChannel = unsigned char;
    struct Color
    {
        ColorChannel r;
        ColorChannel g;
        ColorChannel b;

        static constexpr ColorChannel MAX = 0xFF;
        static constexpr ColorChannel MIN = 0;

        template<typename T>
        operator Vec3<T>() const
        {
            return Vec3<T>({ static_cast<T>(r), static_cast<T>(g), static_cast<T>(b) });
        }

        template<typename T>
        operator Vec4<T>() const
        {
            return Vec4<T>({ static_cast<T>(r), static_cast<T>(g), static_cast<T>(b), static_cast<T>(MAX) });
        }

        static inline ColorChannel boundColorChannel(double newColorChannel)
        {
            return static_cast<unsigned char>((std::max)((std::min)(newColorChannel, static_cast<double>(Color::MAX)),
                static_cast<double>(Color::MIN)));
        }
    };
}