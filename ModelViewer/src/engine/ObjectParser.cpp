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
            int currentObjectIndex = -1;
            ParsedObject obj = {};

            for (std::wstring line; std::getline(in, line); )
            {
                if (std::wstring_view(line.c_str(), 2) == L"v ")
                {
                    obj.vertices.push_back(Vector4<double>(std::move(parseV(line))));
                }

                if (std::wstring_view(line.c_str(), 2) == L"f ")
                {
                    auto f = parseF(line);

                    for (const auto& a: f)
                        obj.indices.push_back(a[0]);
                }
            }

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
            size_t offset = 2;

            while (true)
            {
                auto pos = str.find(L' ', offset);

                if (pos == std::wstring::npos)
                    break;

                f.push_back(parseFSingle(str.substr(offset, pos - offset)));

                offset = pos + 1;
            }

            f.push_back(parseFSingle(str.substr(offset)));

            return f;
        }

        Vector4<double> ObjectParser::parseV(const std::wstring& str)
        {
            size_t offset = 2;
            Vector4<double> vertices({0.0, 0.0, 0.0, 1.0});
            size_t countVertices = 0;

            for (int i = 0; i < 3; i++)
            {
                auto pos = str.find(L' ', offset);

                if (pos == std::wstring::npos)
                    break;

                vertices[i] = std::stod(str.substr(offset, pos - offset));

                offset = pos + 1;
                countVertices++;
            }

            vertices[countVertices] = std::stod(str.substr(offset));

            return vertices;
        }
    }
}


