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

        inline const T& operator()(int column, int row) const
        {
            expect(column >= 0 && column < Size);
            expect(row >= 0 && row < Size);

            return m_data[row * Size + column];
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

        std::size_t size() const
        {
            return Size;
        }

        template<typename E, std::size_t NewSize>
        operator Matrix<E, NewSize>() const
        {
            Matrix<E, NewSize> matrix;

            if (Size < NewSize)
            {
                for (std::size_t i = 0; i < Size; i++)
                    for (std::size_t j = 0; j < Size; j++)
                        matrix(i, j) = static_cast<E>((*this)(i, j));
            }
            else
            {
                for (int i = 0; i < NewSize; i++)
                    for (int j = 0; j < NewSize; j++)
                        matrix(i, j) = static_cast<E>((*this)(i, j));
            }
            
            return matrix;
        }

        // Function to calculate and store inverse, returns false if 
        // matrix is singular 
        Matrix inverse() const
        {
            Matrix inverse;

            T A[Size][Size];

            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    A[i][j] = (*this)(i, j);

            // Find determinant of A[][] 
            T det = determinant(A, Size);
            if (det == 0)
                throw std::exception("could not find inverse matrix: determinant is zero");
            
            // Find adjoint 
            T adj[Size][Size];
            adjoint(A, adj);

            // Find Inverse using formula "inverse(A) = adj(A)/det(A)" 
            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    inverse(i, j) = static_cast<T>(adj[i][j] / static_cast<double>(det));

            return inverse;
        }

        Matrix transpose() const noexcept
        {
            Matrix t;

            for (std::size_t i = 0; i < Size; i++)
                for (std::size_t j = 0; j < Size; j++)
                    t(j, i) = (*this)(i, j);

            return t;
        }

    private:
        void getCofactor(T A[Size][Size], T temp[Size][Size], std::size_t p, std::size_t q, std::size_t n) const
        {
            int i = 0, j = 0;

            // Looping for each element of the matrix 
            for (std::size_t row = 0; row < n; row++)
            {
                for (std::size_t col = 0; col < n; col++)
                {
                    //  Copying into temporary matrix only those element 
                    //  which are not in given row and column 
                    if (row != p && col != q)
                    {
                        temp[i][j++] = A[row][col];

                        // Row is filled, so increase row index and 
                        // reset col index 
                        if (j == n - 1)
                        {
                            j = 0;
                            i++;
                        }
                    }
                }
            }
        }

        /* Recursive function for finding determinant of matrix.
           n is current dimension of A[][]. */
        T determinant(T A[Size][Size], std::size_t n) const
        {
            T D = 0; // Initialize result 

            //  Base case : if matrix contains single element 
            if (n == 1)
                return A[0][0];

            T temp[Size][Size]; // To store cofactors 

            T sign = 1;  // To store sign multiplier 

            // Iterate for each element of first row 
            for (std::size_t f = 0; f < n; f++)
            {
                // Getting Cofactor of A[0][f] 
                getCofactor(A, temp, 0, f, n);
                D += sign * A[0][f] * determinant(temp, n - 1);

                // terms are to be added with alternate sign 
                sign = -sign;
            }

            return D;
        }

        // Function to get adjoint of A[N][N] in adj[N][N]. 
        void adjoint(T A[Size][Size], T adj[Size][Size]) const
        {
            if (Size == 1)
            {
                adj[0][0] = 1;
                return;
            }

            // temp is used to store cofactors of A[][] 
            T sign = 1, temp[Size][Size];

            for (std::size_t i = 0; i < Size; i++)
            {
                for (std::size_t j = 0; j < Size; j++)
                {
                    // Get cofactor of A[i][j] 
                    getCofactor(A, temp, i, j, Size);

                    // sign of adj[j][i] positive if sum of row 
                    // and column indexes is even. 
                    sign = ((i + j) % 2 == 0) ? 1 : -1;

                    // Interchanging rows and columns to get the 
                    // transpose of the cofactor matrix 
                    adj[j][i] = (sign) * (determinant(temp, Size - 1));
                }
            }
        }

    private:
        std::array<T, Size * Size> m_data = {};
    };

    template <typename T>
    using Matrix3 = Matrix<T, 3>;

    template <typename T>
    using Mat3 = Matrix3<T>;

    template<typename T>
    using Matrix4 = Matrix<T, 4>;
    
    template<typename T>
    using Mat4 = Matrix4<T>;
}