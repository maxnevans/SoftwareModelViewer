#pragma once
#include "pch.h"
#include "math/Geometry.h"
#include "ModelViewerApp.h"

namespace ModelViewer
{
    class MainWindow
    {
    private:
        class MainWindowRegisterer
        {
        public:
            MainWindowRegisterer(HINSTANCE hInst, const wchar_t* className);
            ~MainWindowRegisterer();
            DWORD getErrorCode() const;
            bool didSuccess() const;
            HINSTANCE getHInstance() const;

        private:
            std::wstring m_ClassName = L"";
            HINSTANCE m_HInstance = NULL;
            bool m_DidSuccess = false;
            DWORD m_ErrorCode = ERROR_SUCCESS;
        };

    public:
        MainWindow(int x, int y, int width, int height);
        MainWindow(const MainWindow&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;
        void close();

    private:
        // Window procedures
        static LRESULT CALLBACK p_WndInstallProc(HWND, UINT, WPARAM, LPARAM);
        static LRESULT CALLBACK p_WndProxyProc(HWND, UINT, WPARAM, LPARAM);
        LRESULT p_WndProc(UINT, WPARAM, LPARAM);

        // Window message handlers
        LRESULT p_WmCreate(UINT, WPARAM, LPARAM);
        LRESULT p_WmPaint(UINT, WPARAM, LPARAM);
        LRESULT p_WmSizing(UINT, WPARAM, LPARAM);
        LRESULT p_WmClose(UINT, WPARAM, LPARAM);
        LRESULT p_WmDestroy(UINT, WPARAM, LPARAM);
        LRESULT p_WmEraseBackground(UINT, WPARAM, LPARAM);
        LRESULT p_WmModelViewer(UINT, WPARAM, LPARAM);
        LRESULT p_WmKeyDown(UINT, WPARAM, LPARAM);

    private:
        static constexpr const wchar_t* WINDOW_NAME = L"Model Viewer";
        static constexpr const wchar_t* WINDOW_CLASS = L"ModelViewerWindow";
        static constexpr const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        static MainWindowRegisterer m_WndRegisterer;

        HWND m_HWnd = NULL;
        ModelViewerApp m_App;

        // Client rect anchor point
        IntPoint m_AnchorPoint;

        // Client rect dimentions
        IntDimensions m_Dimensions;

        bool m_IsClosingWindow = false;
    };
}