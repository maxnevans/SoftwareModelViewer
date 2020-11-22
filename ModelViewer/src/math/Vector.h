#pragma once
#include "pch.h"
#include "Matrix.h"

namespace ModelViewer
{
    template<typename T, std::size_t Size>
    class Vector
    {
    public:
        Vector() = default;

        Vector(std::array<T, Size> data)
            :
            m_data(std::move(data))
        {}

        Vector<double, Size> normalize() const
        {
            const double len = length();

            if (len > 0.9999995 && len < 1.0000005)
                return *this;

            auto out = *this;

            for (int i = 0; i < Size; i++)
                out.m_data[i] /= len;

            return out;
        }

        double length() const
        {
            return std::sqrt(std::reduce(std::begin(m_data), std::end(m_data), 0.0, [](double acc, T item) { return acc + item * item; }));
        }

        size_t size() const
        {
            return this->m_data.size();
        }

        Vector& normalizeSelf()
        {
            const auto len = length();

            for (int i = 0; i < Size; i++)
                m_data[i] /= len;

            return *this;
        }

        T dotProduct(const Vector& vec) const
        {
            T sum = 0;
            for (int i = 0; i < Size; i++)
                sum += m_data[i] * vec[i];

            return sum;
        }

        double cos(const Vector& vec) const
        {
            return dotProduct(vec) / (length() * vec.length());
        }

        Vector<T, 3> crossProduct(const Vector<T, 3>& vec, bool rowOriented = false) const
        {
            return Vector<T, 3>({
                m_data[1] * vec.m_data[2] - m_data[2] * vec.m_data[1],
                m_data[2] * vec.m_data[0] - m_data[0] * vec.m_data[2],
                m_data[0] * vec.m_data[1] - m_data[1] * vec.m_data[0]
            });
        }

        T& operator[](size_t index)
        {
            return m_data[index];
        }

        const T& operator[](size_t index) const
        {
            return m_data[index];
        }

        Vector operator*(const Vector& vec) const
        {
            static_assert(false);
        }

        Vector operator*(T number) const
        {
            auto output = m_data;
            for (int i = 0; i < Size; i++)
                output[i] *= number;

            return Vector(std::move(output));
        }

        Vector& operator*=(T number)
        {
            for (int i = 0; i < Size; i++)
                m_data[i] *= number;

            return *this;
        }

        Vector operator/(T number) const
        {
            expect(number != 0);

            auto output = m_data;
            for (int i = 0; i < Size; i++)
                output[i] /= number;

            return Vector(std::move(output));
        }

        Vector& operator/=(T number)
        {
            expect(number != 0);

            for (int i = 0; i < Size; i++)
                m_data[i] /= number;

            return *this;
        }

        Vector operator+(const Vector& vec) const
        {
            auto output = m_data;
            for (int i = 0; i < Size; i++)
                output[i] += vec[i];

            return Vector(std::move(output));
        }

        Vector& operator+=(const Vector& vec)
        {
            for (int i = 0; i < Size; i++)
                m_data[i] += vec[i];

            return *this;
        }

        Vector operator-(const Vector& vec) const
        {
            auto output = m_data;
            for (int i = 0; i < Size; i++)
                output[i] -= vec[i];

            return Vector(std::move(output));
        }

        Vector& operator-=(const Vector& vec)
        {
            for (int i = 0; i < Size; i++)
                m_data[i] -= vec[i];

            return *this;
        } 

        template<typename E>
        operator Vector<E, Size>() const
        {
            std::array<E, Size> out;
            for (int i = 0; i < Size; i++)
                out[i] = static_cast<E>(m_data[i]);
            return Vector<E, Size>(std::move(out));
        }

        operator Vector<T, 4>() const
        {
            expect(Size == 3);

            Vector<T, 4> output;
            for (int i = 0; i < Size; i++)
                output[i] = m_data[i];
            return output;
        }

        template<std::size_t NewSize>
        operator Vector<T, NewSize>() const
        {
            if (NewSize < Size)
            {
                Vector<T, NewSize> out;
                for (int i = 0; i < NewSize; i++)
                    out[i] = m_data[i];
                return out;
            }
            else
            {
                Vector<T, NewSize> out(std::array<T, NewSize>{});
                for (int i = 0; i < Size; i++)
                    out[i] = m_data[i];
                return out;
            }
        }

    private:
        std::array<T, Size> m_data;
    };

    template<typename T>
    using Vector4 = Vector<T, 4>;

    template<typename T>
    using Vector3 = Vector<T, 3>;

    template<typename T>
    using Vec2 = Vector<T, 2>;

    enum CoordinateNames
    {
        X, Y, Z, W
    };

    template<typename T>
    Vector4<T> operator*(const Matrix4<T>& matrix, const Vector4<T>& vec)
    {
        Vector4<T> output = {};

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                output[i] += matrix(j, i) * vec[j];

        return output;
    }

    template<typename T>
    Vector4<T> operator*(const Matrix4<T>& matrix, const Vector3<T>& vec)
    {
        return matrix * static_cast<const Vector4<T>&>(vec);
    }
}