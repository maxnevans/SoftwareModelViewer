#include "pch.h"
#include "ObjectParser.h"


namespace ModelViewer
{
    namespace Engine
    {
        ObjectParser::ObjectParser(std::wstring filename)
            :
            m_ObjFile(std::move(filename))
        {
        }

        ParsedObject ObjectParser::parse()
        {
            std::wifstream in(m_ObjFile);
            ParsedObject obj = {};
            std::vector<SignedIndex> signedIndices;

            for (std::wstring line; std::getline(in, line); )
            {
                if (std::wstring_view(line.c_str(), 2) == L"v ")
                {
                    obj.vertices.push_back(Vec4<double>(std::move(parseV(line))));
                } 
                else if (std::wstring_view(line.c_str(), 3) == L"vn ")
                {
                    obj.normals.push_back(Vec3<double>(std::move(parseVN(line))));
                } 
                else if (std::wstring_view(line.c_str(), 2) == L"f ")
                {
                    auto f = parseF(line);

                    for (const auto& a: f)
                        signedIndices.push_back({a[0], a[1], a[2]});
                }
            }

            obj.indices = convertToUnsignedIndices(signedIndices, obj.vertices.size(), 0, obj.normals.size());

            return obj;
        }

        std::array<int, 3> ObjectParser::parseFSingle(const std::wstring& str) const
        {
            constexpr const size_t MAX_F_INDICES = 3;
            std::array<int, MAX_F_INDICES> f = {};
            size_t offset = 0;

            for (size_t i = 0; i < MAX_F_INDICES - 1; i++)
            {
                auto pos = str.find(L'/', offset);

                if (pos == std::wstring::npos)
                    break;

                f[i] = std::stoi(str.substr(offset, pos - offset));
                offset = pos + 1;
            }

            f[MAX_F_INDICES - 1] = std::stoi(str.substr(offset));

            return f;
        }

        std::vector<std::array<int, 3>> ObjectParser::parseF(const std::wstring& str) const
        {
            std::vector<std::array<int, 3>> f = {};
            size_t offsetFromTheBeginingOfTheLine = 2;

            // Skip spaces after "f ", prevent from parsing empty string
            while (offsetFromTheBeginingOfTheLine < str.size() && str[offsetFromTheBeginingOfTheLine] == L' ')
            {
                offsetFromTheBeginingOfTheLine++;
                if (offsetFromTheBeginingOfTheLine == str.size())
                    offsetFromTheBeginingOfTheLine = std::wstring::npos;
            }

            if (offsetFromTheBeginingOfTheLine == std::wstring::npos)
                return f;

            while (true)
            {
                auto pos = str.find(L' ', offsetFromTheBeginingOfTheLine);

                if (pos == std::wstring::npos)
                    break;

                // Skip spaces
                while (pos + 1 < str.size() && str[pos + 1] == L' ')
                {
                    pos++;
                    if (pos + 1 == str.size())
                        pos = std::wstring::npos;
                }

                if (pos == std::wstring::npos)
                    break;

                f.push_back(parseFSingle(str.substr(offsetFromTheBeginingOfTheLine, pos - offsetFromTheBeginingOfTheLine)));

                offsetFromTheBeginingOfTheLine = pos + 1;
            }

            f.push_back(parseFSingle(str.substr(offsetFromTheBeginingOfTheLine)));

            return f;
        }

        Vec4<double> ObjectParser::parseV(const std::wstring& str)
        {
            constexpr int MAX_COUNT_COORDS = 4;
            size_t offsetFromTheBeginingOfTheLine = 2;
            Vec4<double> vertex({0.0, 0.0, 0.0, 1.0});
            size_t countCoords = 0;

            // Skip spaces after "v ", prevent from parsing empty string
            while (offsetFromTheBeginingOfTheLine < str.size() && str[offsetFromTheBeginingOfTheLine] == L' ')
            {
                offsetFromTheBeginingOfTheLine++;
                if (offsetFromTheBeginingOfTheLine == str.size())
                    offsetFromTheBeginingOfTheLine = std::wstring::npos;
            }

            if (offsetFromTheBeginingOfTheLine == std::wstring::npos)
                return vertex;

            for (int i = 0; i < MAX_COUNT_COORDS - 1; i++)
            {
                auto pos = str.find(L' ', offsetFromTheBeginingOfTheLine);

                if (pos == std::wstring::npos)
                    break;

                // Skip spaces
                while (pos + 1 < str.size() && str[pos + 1] == L' ')
                {
                    pos++;
                    if (pos + 1 == str.size())
                        pos = std::wstring::npos;
                }

                if (pos == std::wstring::npos)
                    break;

                vertex[i] = std::stod(str.substr(offsetFromTheBeginingOfTheLine, pos - offsetFromTheBeginingOfTheLine));

                offsetFromTheBeginingOfTheLine = pos + 1;
                countCoords++;
            }

            vertex[countCoords] = std::stod(str.substr(offsetFromTheBeginingOfTheLine));

            return vertex;
        }

        Vec3<double> ObjectParser::parseVN(const std::wstring& str)
        {
            constexpr int MAX_COUNT_COORDS = 3;
            size_t offsetFromTheBeginingOfTheLine = 3;
            Vec3<double> normal{};
            size_t countCoords = 0;

            // Skip spaces after "vn ", prevent from parsing empty string
            while (offsetFromTheBeginingOfTheLine < str.size() && str[offsetFromTheBeginingOfTheLine] == L' ')
            {
                offsetFromTheBeginingOfTheLine++;
                if (offsetFromTheBeginingOfTheLine == str.size())
                    offsetFromTheBeginingOfTheLine = std::wstring::npos;
            }

            if (offsetFromTheBeginingOfTheLine == std::wstring::npos)
                return normal;

            for (int i = 0; i < MAX_COUNT_COORDS - 1; i++)
            {
                auto pos = str.find(L' ', offsetFromTheBeginingOfTheLine);

                if (pos == std::wstring::npos)
                    break;

                // Skip spaces
                while (pos + 1 < str.size() && str[pos + 1] == L' ')
                {
                    pos++;
                    if (pos + 1 == str.size())
                        pos = std::wstring::npos;
                }

                if (pos == std::wstring::npos)
                    break;

                normal[i] = std::stod(str.substr(offsetFromTheBeginingOfTheLine, pos - offsetFromTheBeginingOfTheLine));

                offsetFromTheBeginingOfTheLine = pos + 1;
                countCoords++;
            }

            normal[countCoords] = std::stod(str.substr(offsetFromTheBeginingOfTheLine));

            return normal;
        }

        std::vector<Index> ObjectParser::convertToUnsignedIndices(const std::vector<SignedIndex>& signedIndices,
            std::size_t verticesCount, std::size_t textureVerticesCount, std::size_t normalsCount) const
        {
            std::vector<Index> indices(signedIndices.size());

            for (std::size_t i = 0; i < signedIndices.size(); i++)
            {
                indices[i].vertex = signedIndices[i].vertex < 0 ? verticesCount + signedIndices[i].vertex : signedIndices[i].vertex - 1;
                indices[i].texture = signedIndices[i].texture < 0 ? textureVerticesCount + signedIndices[i].texture : signedIndices[i].texture - 1;
                indices[i].normal = signedIndices[i].normal < 0 ? normalsCount + signedIndices[i].normal : signedIndices[i].normal - 1;
            }

            return indices;
        }
    }
}


