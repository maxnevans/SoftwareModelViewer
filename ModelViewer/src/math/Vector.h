#pragma once
#include "pch.h"
#include "Matrix.h"

namespace ModelViewer
{
    template<typename T>
    class Vector
        :
        public Matrix<T>
    {
    public:
        Vector() = default;

        Vector(Matrix<T> matrix)
            :
            Matrix<T>(std::move(matrix))
        {
            expect(this->m_NumberRows == 1 || this->m_NumberColumns == 1);
        }

        Vector(std::valarray<T> data, bool rowOriented = false)
            :
            Matrix<T>(std::move(data), 1, data.size())
        {
            if (rowOriented)
            {
                this->m_NumberRows = this->m_NumberColumns;
                this->m_NumberColumns = 1;
            }
        }

        Vector& rotateToColumn()
        {
            this->m_NumberRows = this->m_Data.size();
            this->m_NumberColumns = 1;
            return *this;
        }

        Vector& rotateToRow()
        {
            this->m_NumberRows = 1;
            this->m_NumberColumns = this->m_Data.size();
            return *this;
        }

        Vector<double> normalize() const
        {
            if (this->m_Data.size() == 0)
                return Vector<double>{};

            double length = std::sqrt((this->m_Data * this->m_Data).sum());

            if (length > 0.9999995 && length < 1.0000005)
                return *this;

            std::valarray<double> copy(this->m_Data.size());
            std::copy(std::begin(this->m_Data), std::end(this->m_Data), std::begin(copy));
            copy /= length;
            return copy;
        }

        double length() const
        {
            return std::sqrt((this->m_Data * this->m_Data).sum());
        }

        size_t size() const
        {
            return this->m_Data.size();
        }

        Vector& normalizeSelf()
        {
            if (this->m_Data.size() == 0)
                return *this;

            auto length = length();
            this->m_Data /= length;
            return *this;
        }

        T dotProduct(const Vector& vec) const
        {
            expect(this->m_Data.size() > 0 && vec.m_Data.size() > 0);
            
            return (this->m_Data * vec.m_Data).sum();
        }

        double cos(const Vector& vec) const
        {
            expect(vec.m_Data.size() > 0 && this->m_Data.size() > 0);

            return dotProduct(vec) / (length() * vec.length());
        }

        Vector crossProduct(const Vector& vec, bool rowOriented = false) const
        {
            expect(this->m_Data.size() == 3 && vec.m_Data.size() == 3);

            return Vector({
                this->m_Data[1] * vec.m_Data[2] - this->m_Data[2] * vec.m_Data[1],
                this->m_Data[2] * vec.m_Data[0] - this->m_Data[0] * vec.m_Data[2],
                this->m_Data[0] * vec.m_Data[1] - this->m_Data[1] * vec.m_Data[0]
            }, rowOriented);
        }

        T& operator[](size_t index)
        {
            expect(index < this->m_Data.size());
            return this->m_Data[index];
        }

        const T& operator[](size_t index) const
        {
            expect(index < this->m_Data.size());
            return this->m_Data[index];
        }

        T& at(size_t index)
        {
            if (index > this->m_Data.size())
                throw std::out_of_range("vector index out of range");

            return this->m_Data[index];
        }

        Vector operator*(const Vector& vec) const
        {
            static_assert(false);
        }

        Vector operator*(const Matrix<T>& matrix) const
        {
            return Vector<T>(std::move(Matrix<T>::operator * (matrix)));
        }

        Vector operator/(T number) const
        {
            expect(number != 0);
            return Vector(this->m_Data / number, this->m_NumberRows > 1);
        }

        Vector& operator/=(T number)
        {
            expect(number != 0);
            this->m_Data /= number;
            return *this;
        }

        Vector operator+(const Vector& vec) const
        {
            expect(vec.m_Data.size() == this->m_Data.size());

            return Vector(this->m_Data + vec.m_Data, this->m_NumberRows > 1);
        }

        Vector& operator+=(const Vector& vec)
        {
            expect(vec.m_Data.size() == this->m_Data.size());

            this->m_Data += vec.m_Data;
            return *this;
        }

        Vector operator-(const Vector& vec) const
        {
            expect(vec.m_Data.size() == this->m_Data.size());

            return Vector(this->m_Data - vec.m_Data, this->m_NumberRows > 1);
        }

        Vector& operator-=(const Vector& vec)
        {
            expect(vec.m_Data.size() == this->m_Data.size());

            this->m_Data -= vec.m_Data;
            return *this;
        }

        template<typename E>
        Vector<E> staticCast() const
        {
            std::valarray<E> arr(this->m_Data.size());
            for (size_t i = 0; i < this->m_Data.size(); i++)
            {
                arr[i] = static_cast<E>(this->m_Data[i]);
            }
            return Vector<E>(std::move(arr), this->m_NumberRows > 1);
        }
 
    };

    template<typename T>
    Vector<T> operator*(const Matrix<T>& matrix, const Vector<T>& vec)
    {
        expect(matrix.getNumberColumns() == vec.getNumberRows());
        expect(vec.getNumberColumns() == 1);

        if (matrix.getNumberRows() == 0 || vec.getNumberColumns() == 0)
            return Vector<T>{};

        return Vector<T>{matrix * static_cast<Matrix<T>>(vec)};
    }
}