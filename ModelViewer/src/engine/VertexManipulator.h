#pragma once
#include "pch.h"

namespace ModelViewer
{
    namespace Engine
    {
        class VertexManipulator
        {
        public:
            void setVerticies(std::vector<double> vertexBuffer, std::vector<int> vertexIndicies);
            void setMVPMatrix();

        private:
            std::vector<double> m_VertexBuffer;
            std::vector<int> m_VertexIndicies;
        };
    }
}