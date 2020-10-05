#pragma once
#include "pch.h"
#include "Core.h"
#include "math/Matrix.h"
#include "math/Vector.h"

namespace ModelViewer
{
    namespace Engine
    {
        template<typename T>
        Matrix4<T> createTranslateMatrix(const Vector4<T>& translateAmount)
        {
            return Matrix4<T>({
                1,      0,      0,      translateAmount[0],
                0,      1,      0,      translateAmount[1],
                0,      0,      1,      translateAmount[2],
                0,      0,      0,      1
            });
        }

        template<typename T>
        Matrix4<T> createScaleMatrix(const Vector4<T>& scaleAmount)
        {
            return Matrix4<T>({
                scaleAmount[0], 0,              0,              0,
                0,              scaleAmount[1], 0,              0,
                0,              0,              scaleAmount[2], 0,
                0,              0,              0,              1
            });
        }

        Matrix4<double> createRotateXMatrix(double angle)
        {
            return Matrix4<double>({
                1,      0,                  0,                  0,
                0,      std::cos(angle),    -std::sin(angle),   0,
                0,      std::sin(angle),    std::cos(angle),    0,
                0,      0,                  0,                  1
            });
        }

        Matrix4<double> createRotateYMatrix(double angle)
        {
            return Matrix4<double>({
                std::cos(angle),    0,      std::sin(angle),        0,
                0,                  1,      0,                      0,
                -std::sin(angle),   0,      std::cos(angle),        0,
                0,                  0,      0,                      1
            });
        }

        Matrix4<double> createRotateZMatrix(double angle)
        {
            return Matrix4<double>({
                std::cos(angle),    -std::sin(angle),       0,      0,
                std::sin(angle),    std::cos(angle),        0,      0,
                0,                  0,                      1,      0,
                0,                  0,                      0,      1
            });
        }

        template<typename T>
        Vector4<T> translate(const Vector4<T>& vec, T x, T y, T z)
        {
            return vec * createTranslateMatrix(Vector4<T>({ x, y, z }));
        }

        template<typename T>
        Vector4<T> scale(const Vector4<T>& vec, T xScale, T yScale, T zScale)
        {
            return vec * createScaleMatrix(Vector4<T>({ xScale, yScale, zScale }));
        }

        template<typename T>
        Vector4<T> rotateX(const Vector4<T>& vec, double angle)
        {
            return vec * createRotateXMatrix(angle);
        }

        template<typename T>
        Vector4<T> rotateY(const Vector4<T>& vec, double angle)
        {
            return vec * createRotateYMatrix(angle);
        }

        template<typename T>
        Vector4<T> rotateZ(const Vector4<T>& vec, double angle)
        {
            return vec * createRotateZMatrix(angle);
        }
    }
}