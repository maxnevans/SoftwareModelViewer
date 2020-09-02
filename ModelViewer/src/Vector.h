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
    };
}