#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer
{
    namespace Engine
    {
        struct Index
        {
            int vertex;
            int texture;
            int normal;
        };

        struct ParsedObject
        {
            std::vector<Vec4<double>> vertices;
            std::vector<Vec3<double>> normals;
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
            
        private:
            std::wstring m_ObjFile;
            bool m_Parsed = false;
        };
    }
}