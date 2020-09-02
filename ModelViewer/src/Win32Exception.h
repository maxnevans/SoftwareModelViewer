#pragma once
#include "pch.h"
#include "Exception.h"

namespace ModelViewer
{
    class Win32Exception
        :
        public Exception
    {
    public:
        Win32Exception(const std::wstring& functionName)
            :
            Win32Exception(functionName, L"", GetLastError())
        {
        }

        Win32Exception(const std::wstring& functionName, DWORD code)
            :
            Win32Exception(functionName, L"", code)
        {
        }

        Win32Exception(const std::wstring& functionName, const std::wstring& message)
            :
            Win32Exception(functionName, message, GetLastError())
        {
        }

        Win32Exception(const std::wstring& functionName, const std::wstring& message, DWORD code)
            :
            Exception(message),
            m_FunctionName(functionName),
            m_ErrorCode(code)
        {
            std::wstringstream ss;
            ss << L"Error in " << m_FunctionName << L" ";
            ss << L"[System Code: " << m_ErrorCode << "]";

            if (!getMessage().empty())
                ss << ": " << getMessage();

            if (!ss)
                setMessage(L"Fatal error: failed to generate exception message!");
            else
                setMessage(ss.str().c_str());
        }

        DWORD getCode() const
        {
            return m_ErrorCode;
        }

    protected:
        const DWORD& getErrorCode() const
        {
            return m_ErrorCode;
        }

    private:
        std::wstring m_FunctionName = L"";
        DWORD m_ErrorCode = ERROR_SUCCESS;
    };
}
