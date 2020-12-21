#include "pch.h"
#include "TextureParser.h"
#include "lodepng/lodepng.h"

namespace ModelViewer::Engine
{
    TextureParser::TextureParser(std::string filename)
        :
        m_filename(filename)
    {
    }

    ParsedTexture TextureParser::parse()
    {
        ParsedTexture out = {};

        unsigned err = lodepng::decode(out.rawData, out.width, out.height, m_filename);

        if (err)
        {
            std::stringstream ss;
            ss << "decode error " << err << ": " << lodepng_error_text(err);

            throw std::runtime_error(ss.str().c_str());
        }

        return out;
    }
}
