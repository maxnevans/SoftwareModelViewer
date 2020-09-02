#pragma once
#include "pch.h"

namespace ModelViewer
{
    class Exception
    {
    public:
        Exception() {}

        Exception(const std::wstring& msg)
        {
            m_Message = msg;
        }

        virtual std::wstring what() const noexcept
        {
            return m_Message;
        }

    protected:
        const std::wstring& getMessage() const
        {
            return m_Message;
        }

        void setMessage(const std::wstring& msg)
        {
            m_Message = msg;
        }

    private:
        std::wstring m_Message = L"";
    };
}
