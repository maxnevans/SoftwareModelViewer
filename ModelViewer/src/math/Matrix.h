#pragma once
#include "pch.h"
#include "Core.h"

namespace ModelViewer
{
    template<typename T>
    class Matrix
    {
    public:   
        Matrix() = default;

        Matrix(std::valarray<T> data, size_t numberRows, size_t numberColumns)
            :
            m_Data(std::move(data)),
            m_NumberRows(numberRows),
            m_NumberColumns(numberColumns)
        {
        }

        Matrix operator*(const Matrix& matrix) const
        {
            expect(m_NumberColumns == matrix.m_NumberRows);

            if (m_NumberRows == 0 || matrix.m_NumberRows == 0 || matrix.m_NumberColumns == 0)
                return Matrix{};

            std::valarray<T> out(m_NumberRows * matrix.m_NumberColumns);

            for (size_t row = 0; row < m_NumberRows; row++)
            {
                for (size_t col = 0; col < matrix.m_NumberColumns; col++)
                {
                    out[row * matrix.m_NumberColumns + col] = (m_Data[std::slice(row * m_NumberColumns, m_NumberColumns, 1)]
                        * matrix.m_Data[std::slice(col, matrix.m_NumberRows, matrix.m_NumberColumns)]).sum();
                }
            }

            return Matrix(std::move(out), m_NumberRows, matrix.m_NumberColumns);
        }

        Matrix& operator*=(const Matrix& matrix)
        {
            if (m_Data.size() == 0)
                return *this;

            *this = *this * matrix;
            return *this;
        }

        Matrix operator*(T number)
        {
            return Matrix(m_Data * number, m_NumberRows, m_NumberColumns);
        }

        Matrix& operator*=(T number)
        {
            if (m_Data.size() == 0)
                return *this;

            m_Data *= number;
            return *this;
        }

        Matrix operator+(const Matrix& matrix)
        {
            expect(m_NumberRows == matrix.m_NumberRows && m_NumberColumns == matrix.m_NumberColumns);
            return Matrix(m_Data + matrix.m_Data, m_NumberRows, m_NumberColumns);
        }

        Matrix& operator+=(const Matrix& matrix)
        {
            expect(m_NumberRows == matrix.m_NumberRows && m_NumberColumns == matrix.m_NumberColumns);
            m_Data += matrix.m_Data;
            return *this;
        }

        inline T& at(size_t row, size_t column)
        {
            if (row >= m_NumberRows)
                throw std::out_of_range("matrix row index out of range");

            if (column >= m_NumberColumns)
                throw std::out_of_range("matrix column index out of range");

            return m_Data[row * m_NumberColumns + column];
        }

    protected:
        size_t m_NumberColumns = 0;
        size_t m_NumberRows = 0;

        std::valarray<T> m_Data = {};
    };
}