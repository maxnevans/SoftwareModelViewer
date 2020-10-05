#pragma once
#include "pch.h"
#include "math/Vector.h"

namespace ModelViewer
{
    namespace Engine
    {
        struct ParsedObject
        {
            std::vector<Vector4<double>> vertices;
            std::vector<int> indices;
        };

        class ObjectParser
        {
        public:
            ObjectParser(std::wstring filename);
            ParsedObject parse();

        private:
            std::array<int, 3> parseFSingle(const std::wstring& str) const;
            std::vector<std::array<int, 3>> parseF(const std::wstring& str) const;
            Vector4<double> parseV(const std::wstring& str);
            
        private:
            std::wstring m_ObjFile;
            bool m_Parsed = false;
        };
    }
}