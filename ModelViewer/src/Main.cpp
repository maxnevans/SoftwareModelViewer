#include "pch.h"
#include "Exception.h"
#include "Win32Exception.h"
#include "MainWindow.h"

#define EXECEPTION(wStrMsg) MessageBox(NULL, (wStrMsg), L"Critical Error", MB_OK | MB_ICONERROR)

constexpr int WINDOW_WIDTH = 720;
constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_START_POS_X = 300;
constexpr int WINDOW_START_POS_Y = 300;

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    Gdiplus::GdiplusStartupInput input;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &input, nullptr);

    try
    {
        ModelViewer::MainWindow window(WINDOW_START_POS_X, WINDOW_START_POS_Y, WINDOW_WIDTH, WINDOW_HEIGHT);

        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0))
        {
            DispatchMessage(&msg);
        }

        Gdiplus::GdiplusShutdown(gdiplusToken);
        return static_cast<int>(msg.wParam);
    }
    catch (const ModelViewer::Win32Exception& e)
    {
        EXECEPTION(e.what().c_str());
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return -1;
    }
    catch (const ModelViewer::Exception& e)
    {
        EXECEPTION(e.what().c_str());
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return -1;
    }
    catch (...)
    {
        EXECEPTION(L"Unknown exception happend!");
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return -1;
    }

    return 0;
}