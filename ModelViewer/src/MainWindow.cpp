#include "pch.h"
#include "MainWindow.h"
#include "Win32Exception.h"
#include "Core.h"
#include "ModelViewerWindowMessage.h"


namespace ModelViewer
{
    MainWindow::MainWindowRegisterer MainWindow::m_WndRegisterer = { GetModuleHandle(NULL), MainWindow::WINDOW_CLASS };

    MainWindow::MainWindowRegisterer::MainWindowRegisterer(HINSTANCE hInst, const wchar_t* className)
        :
        m_HInstance(hInst),
        m_ClassName(className)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = MainWindow::p_WndInstallProc;
        wc.hInstance = hInst;
        wc.lpszClassName = className;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.style = CS_HREDRAW | CS_VREDRAW;

        if (RegisterClassEx(&wc) == FALSE)
        {
            m_ErrorCode = GetLastError();
            m_DidSuccess = false;
        }
        else
            m_DidSuccess = true;
    }

    MainWindow::MainWindowRegisterer::~MainWindowRegisterer()
    {
        UnregisterClass(m_ClassName.c_str(), m_HInstance);
    }

    DWORD MainWindow::MainWindowRegisterer::getErrorCode() const
    {
        return m_ErrorCode;
    }

    bool MainWindow::MainWindowRegisterer::didSuccess() const
    {
        return m_DidSuccess;
    }

    HINSTANCE MainWindow::MainWindowRegisterer::getHInstance() const
    {
        return m_HInstance;
    }

    MainWindow::MainWindow(int x, int y, int width, int height)
        :
        m_AnchorPoint({ x, y }),
        m_Dimensions({ width, height }),
        m_App()
    {
        // Adjust window rect to desired client rect

        RECT windowClientRect = {
            m_AnchorPoint.x,
            m_AnchorPoint.y,
            m_AnchorPoint.x + m_Dimensions.width,
            m_AnchorPoint.y + m_Dimensions.height,
        };

        if (AdjustWindowRect(&windowClientRect, WINDOW_STYLE, NULL) == NULL)
            throw Win32Exception(L"AdjustWindowRect");

        HWND hWnd = CreateWindow(WINDOW_CLASS, WINDOW_NAME, WINDOW_STYLE,
            windowClientRect.left, windowClientRect.top,
            windowClientRect.right - windowClientRect.left,
            windowClientRect.bottom - windowClientRect.top,
            NULL, NULL, m_WndRegisterer.getHInstance(), this);

        if (hWnd == NULL)
            throw Win32Exception(L"CreateWindow");

        // Do not set m_HWnd member here. It is being set inside 
        // p_WndInstallProc more earlier that it could be set here.

        m_App.loadMeshFromFile(L"model.obj", [this](bool isLoadedSuccessfully)
        {
            if (!isLoadedSuccessfully)
            {
                MessageBox(m_HWnd, L"Failed to load mesh from file!", L"Error", MB_OK | MB_ICONERROR);
                PostMessage(m_HWnd, ModelViewerWindowMessage::WM_MODELVIEWER, ModelViewerWindowMessage::WPARAM_EXCEPTION, NULL);
                return;
            }
                       
            PostMessage(m_HWnd, ModelViewerWindowMessage::WM_MODELVIEWER, ModelViewerWindowMessage::WPARAM_REDRAW, NULL);
        });
    }

    void MainWindow::close()
    {
        expect(m_HWnd != NULL);

        if (DestroyWindow(m_HWnd) == FALSE)
            throw Win32Exception(L"DestroyWindow");
    }

    LRESULT MainWindow::p_WndInstallProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
            throw Exception(L"Failed to setup p_WndProxyProc! WM_NCCREATE has not been called!");

        if (uMsg != WM_NCCREATE)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        MainWindow* const window = reinterpret_cast<MainWindow* const>(createStruct->lpCreateParams);

        SetLastError(ERROR_SUCCESS);

        window->m_HWnd = hWnd;

        if (SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window)) == FALSE && GetLastError() != ERROR_SUCCESS)
            throw Exception(L"Failed to set GWLP_USERDATA when handling p_WndInstallProc");

        if (SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MainWindow::p_WndProxyProc)) == FALSE && GetLastError() != ERROR_SUCCESS)
            throw Exception(L"Failed to set GWLP_WNDPROC when handling p_WndInstallProc with p_WndProxyProc");

        return window->p_WndProc(uMsg, wParam, lParam);
    }

    LRESULT MainWindow::p_WndProxyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        MainWindow* const wnd = reinterpret_cast<MainWindow* const>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        return wnd->p_WndProc(uMsg, wParam, lParam);
    }

    LRESULT MainWindow::p_WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_CREATE:                                 return p_WmCreate(uMsg, wParam, lParam);
            case WM_SIZING:                                 return p_WmSizing(uMsg, wParam, lParam);
            case WM_PAINT:                                  return p_WmPaint(uMsg, wParam, lParam);
            case WM_ERASEBKGND:                             return p_WmEraseBackground(uMsg, wParam, lParam);
            case WM_CLOSE:                                  return p_WmClose(uMsg, wParam, lParam);
            case WM_DESTROY:                                return p_WmDestroy(uMsg, wParam, lParam);
            case ModelViewerWindowMessage::WM_MODELVIEWER:  return p_WmModelViewer(uMsg, wParam, lParam);
        }

        return DefWindowProc(m_HWnd, uMsg, wParam, lParam);
    }

    LRESULT MainWindow::p_WmCreate(UINT, WPARAM, LPARAM)
    {
        expect(m_HWnd != NULL);

        m_App.start();

        return 0;
    }

    LRESULT MainWindow::p_WmPaint(UINT, WPARAM, LPARAM)
    {
        expect(m_HWnd != NULL);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_HWnd, &ps);
        HDC memDc = CreateCompatibleDC(hdc);
        HBITMAP hBm = CreateCompatibleBitmap(hdc, m_Dimensions.width, m_Dimensions.height);
        SelectObject(memDc, hBm);

        Gdiplus::Graphics gfx(memDc);

        m_App.draw(gfx, { m_Dimensions });

        BitBlt(hdc, 0, 0, m_Dimensions.width, m_Dimensions.height, memDc, 0, 0, SRCCOPY);

        DeleteObject(hBm);
        DeleteObject(memDc);

        return 0;
    }

    LRESULT MainWindow::p_WmSizing(UINT, WPARAM, LPARAM lParam)
    {
        expect(m_HWnd != NULL);

        // Changing saved m_Dimensions and m_AnchorPoint

        RECT clientRect;
        if (GetClientRect(m_HWnd, &clientRect) == FALSE)
            throw Win32Exception(L"GetClientRect");

        m_Dimensions.width = clientRect.right;
        m_Dimensions.height = clientRect.bottom;

        RECT* windowRect = reinterpret_cast<RECT*>(lParam);

        m_AnchorPoint = {
            windowRect->left,
            windowRect->top,
        };

        // Redraw

        InvalidateRect(m_HWnd, NULL, FALSE);

        return TRUE;
    }

    LRESULT MainWindow::p_WmClose(UINT, WPARAM, LPARAM)
    {
        expect(m_HWnd != NULL);

        if (!m_IsClosingWindow && !m_App.didStop())
        {
            // Ignore all close messages if we did not stop app yet

            if (!m_IsClosingWindow)
            {
                m_App.stop(0);
                m_IsClosingWindow = true;

                // Give user notification that application is closing right now
            }

            return 0;
        }

        DestroyWindow(m_HWnd);
        return 0;
    }

    LRESULT MainWindow::p_WmDestroy(UINT, WPARAM, LPARAM)
    {
        PostQuitMessage(0);
        m_HWnd = NULL;
        return 0;
    }

    LRESULT MainWindow::p_WmEraseBackground(UINT, WPARAM, LPARAM)
    {
        return TRUE;
    }

    LRESULT MainWindow::p_WmModelViewer(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        expect(m_HWnd != NULL);

        switch (wParam)
        {
            case ModelViewerWindowMessage::WPARAM_REDRAW:
            {
                InvalidateRect(m_HWnd, NULL, FALSE);
                break;
            }
            case ModelViewerWindowMessage::WPARAM_EXCEPTION:
            {
                MessageBox(m_HWnd, L"ModelViewerApp Exception happend!", L"Model Viewer Exception", MB_OK | MB_ICONERROR);
                m_IsClosingWindow = true;
                DestroyWindow(m_HWnd);
                break;
            }
        }

        return 0;
    }
}