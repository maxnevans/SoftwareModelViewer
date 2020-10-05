#pragma once
#include "pch.h"
#include "Core.h"

namespace ModelViewer
{
    template<typename T, std::size_t Size>
    class Matrix
    {
    public:   
        Matrix() = default;

        Matrix(std::array<T, Size * Size> data)
            :
            m_data(std::move(data))
        {}

        inline T& operator()(int m, int n)
        {
            expect(m >= 0 && m < Size);
            expect(n >= 0 && n < Size);

            return m_data[n * Size + m];
        }

        inline const T& operator()(int m, int n) const
        {
            expect(m >= 0 && m < Size);
            expect(n >= 0 && n < Size);

            return m_data[n * Size + m];
        }

        Matrix operator*(const Matrix& matrix) const
        {
            std::array<T, Size * Size> out = {};

            for (std::size_t i = 0; i < Size * Size; i += Size)
            {
                const auto c = &out[i];
                const auto a = &m_data[i];
                for (std::size_t k = 0; k < Size; k++)
                {
                    const auto aVal = *(a + k);
                    const auto b = &matrix.m_data[k * Size];
                    for (std::size_t j = 0; j < Size; j++)
                        *(c + j) += aVal * *(b + j);
                }
            }

            return out;
        }

        Matrix& operator*=(const Matrix& matrix)
        {
            *this = *this * matrix;
            return *this;
        }

        Matrix& operator*=(T number)
        {
            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    (*this)(i, j) *= number;

            return *this;
        }

        Matrix operator*(T number) const
        {
            auto clone = *this;
            clone *= number;

            return clone;
        }

        Matrix operator/(T number) const
        {
            return *this * (static_cast<T>(1) / number);
        }

        Matrix& operator/=(T number)
        {
            return *this *= (static_cast<T>(1) / number);
        }

        Matrix operator+(const Matrix& matrix) const
        {
            auto out = *this;

            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    out(i, j) += matrix(i, j);

            return out;
        }

        Matrix& operator+=(const Matrix& matrix)
        {
            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    (*this)(i, j) += matrix(i, j);

            return *this;
        }

        Matrix operator+(T number) const
        {
            auto clone = *this;
            clone += number;

            return clone;
        }

        Matrix& operator+=(T number)
        {
            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    (*this)(i, j) += number;

            return *this;
        }

        Matrix operator-(T number) const
        {
            return *this + (-number);
        }

        Matrix& operator-=(T number)
        {
            return *this += -number;
        }

        template<typename E>
        operator Matrix<E, Size>() const
        {
            std::array<E, Size * Size> copy;
            for (int i = 0; i < Size * Size; i++)
                copy[i] = static_cast<E>(m_data[i]);

            return Matrix<E, Size>(std::move(copy));
        }

    private:
        std::array<T, Size * Size> m_data = {};
    };

    template<typename T>
    using Matrix4 = Matrix<T, 4>;
}