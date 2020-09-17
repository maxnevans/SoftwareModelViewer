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
        Matrix<T> createTranslateMatrix(const Vector<T>& translateAmount)
        {
            expect(translateAmount.size() == 3);

            return Matrix<T>({
                1,      0,      0,      translateAmount[0],
                0,      1,      0,      translateAmount[1],
                0,      0,      1,      translateAmount[2],
                0,      0,      0,      1
            }, 4, 4);
        }

        template<typename T>
        Matrix<T> createScaleMatrix(const Vector<T>& scaleAmount)
        {
            expect(scaleAmount.size() == 3);

            return Matrix<T>({
                scaleAmount[0], 0,              0,              0,
                0,              scaleAmount[1], 0,              0,
                0,              0,              scaleAmount[2], 0,
                0,              0,              0,              1
            }, 4, 4);
        }

        Matrix<double> createRotateXMatrix(double angle)
        {
            return Matrix<double>({
                1,      0,                  0,                  0,
                0,      std::cos(angle),    -std::sin(angle),   0,
                0,      std::sin(angle),    std::cos(angle),    0,
                0,      0,                  0,                  1
            }, 4, 4);
        }

        Matrix<double> createRotateYMatrix(double angle)
        {
            return Matrix<double>({
                std::cos(angle),    0,      std::sin(angle),        0,
                0,                  1,      0,                      0,
                -std::sin(angle),   0,      std::cos(angle),        0,
                0,                  0,      0,                      1
            }, 4, 4);
        }

        Matrix<double> createRotateZMatrix(double angle)
        {
            return Matrix<double>({
                std::cos(angle),    -std::sin(angle),       0,      0,
                std::sin(angle),    std::cos(angle),        0,      0,
                0,                  0,                      1,      0,
                0,                  0,                      0,      1
            }, 4, 4);
        }

        template<typename T>
        Vector<T> translate(const Vector<T>& vec, T x, T y, T z)
        {
            expect(vec.size() == 4);

            return vec * createTranslateMatrix(Vector<T>({ x, y, z }));
        }

        template<typename T>
        Vector<T> scale(const Vector<T>& vec, T xScale, T yScale, T zScale)
        {
            expect(vec.size() == 4);

            return vec * createScaleMatrix(Vector<T>({ xScale, yScale, zScale }));
        }

        template<typename T>
        Vector<T> rotateX(const Vector<T>& vec, double angle)
        {
            expect(vec.size() == 4);

            return vec * createRotateXMatrix(angle);
        }

        template<typename T>
        Vector<T> rotateY(const Vector<T>& vec, double angle)
        {
            expect(vec.size() == 4);

            return vec * createRotateYMatrix(angle);
        }

        template<typename T>
        Vector<T> rotateZ(const Vector<T>& vec, double angle)
        {
            expect(vec.size() == 4);

            return vec * createRotateZMatrix(angle);
        }
    }
}