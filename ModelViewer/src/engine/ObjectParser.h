#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer
{
    namespace Engine
    {
        template<typename T>
        struct IndexGeneric
        {
            T vertex;
            T texture;
            T normal;
        };
        using Index = IndexGeneric<std::size_t>;
        using SignedIndex = IndexGeneric<int>;

        struct ParsedObject
        {
            std::vector<Vec4<double>> vertices;
            std::vector<Vec3<double>> normals;
            std::vector<Vec3<double>> textureVertices;
            std::vector<Index> indices;
        };

        class ObjectParser
        {
        public:
            ObjectParser(std::wstring filename);
            ParsedObject parse();

        private:
            std::array<int, 3> parseFSingle(const std::wstring& str) const;
            std::vector<std::array<int, 3>> parseF(const std::wstring& str) const;
            Vec4<double> parseV(const std::wstring& str);
            Vec3<double> parseVN(const std::wstring& str);
            Vec3<double> parseVT(const std::wstring& str);
            std::vector<Index> convertToUnsignedIndices(const std::vector<SignedIndex>& signedIndices, 
                std::size_t verticesCount, std::size_t textureVerticesCount, std::size_t normalsCount) const;
            
        private:
            std::wstring m_ObjFile;
            bool m_Parsed = false;
        };
    }
}