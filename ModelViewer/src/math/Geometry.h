#pragma once

template<typename T>
struct Point
{
    T x;
    T y;
};

using IntPoint = Point<int>;

template<typename T>
struct Dimensions
{
    T width;
    T height;
};

using IntDimensions = Dimensions<int>;